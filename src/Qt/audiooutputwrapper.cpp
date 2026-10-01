#include "audiooutputwrapper.h"
#include "qtel6.h"
#include "p6vxapp.h"

#ifndef NOSOUND
#include <QMediaDevices>
#include <QAudioSink>
#include <QBuffer>
#include <QMutex>
#include <QDebug>
#include <QElapsedTimer>
#include <atomic>

#ifdef NOCALLBACK


AudioOutputWrapper::AudioOutputWrapper(const QAudioDevice &device,
									   const QAudioFormat &format,
									   CBF_SND cbFunc,
									   void *cbData,
									   int samples,
									   QObject *parent)
	: QObject(parent)
	, AudioSink(new QAudioSink(device, format, this))
{
	AudioSink->setBufferSize(samples * bytesPerSample());
}

AudioOutputWrapper::~AudioOutputWrapper()
{
}

void AudioOutputWrapper::start()
{
	AudioBuffer = AudioSink->start();
}

void AudioOutputWrapper::suspend()
{
	AudioSink->suspend();
}

void AudioOutputWrapper::resume()
{
	AudioSink->resume();
}

void AudioOutputWrapper::stop()
{
	AudioSink->stop();
}

void AudioOutputWrapper::writeAudioStream(BYTE *stream, int samples)
{
	if (AudioBuffer){
		AudioBuffer->write(reinterpret_cast<const char*>(stream), samples * bytesPerSample());
	}
}

int AudioOutputWrapper::queuedAudioSamples()
{
	qDebug() << AudioSink->state() << AudioSink->bytesFree();
	if (AudioBuffer){
		return AudioBuffer->bytesAvailable() / bytesPerSample();
	}
}

int AudioOutputWrapper::bytesPerSample()
{
	return AudioSink->format().bytesPerSample();
}

QAudio::State AudioOutputWrapper::state() const
{
	return AudioSink->state();
}

#else

class AudioBufferWrapper : public QIODevice
{
public:
	AudioBufferWrapper(CBF_SND cbFunc,
					   void *cbData,
					   int bytesPerSample,
					   QObject* parent)
		: QIODevice(parent)
		, CbFunc(cbFunc)
		, CbData(cbData)
		, BytesPerSample(bytesPerSample)
	{}

	virtual ~AudioBufferWrapper(){
		close();
	}

	bool isSequential() const override{
		return true;
	}

	qint64 size() const override
	{
		return bytesAvailable();
	}

	qint64 bytesAvailable() const override{
		EL6* el6 = STATIC_CAST(EL6*, CbData);
		QtEL6* qtel6 = dynamic_cast<QtEL6*>(el6);
		qint64 bytesAvailable = qtel6->GetSoundReadySize() * BytesPerSample /
							 ((double)qtel6->GetSpeedRatio() / 100.0);
#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
		// Qt 6.4以前のGStreamerバックエンドは実際に用意されている分しか読まないため
		// 経過時間から再生済み量を推定し、一定量先行して供給する
		if (Clock.isValid()){
			const qint64 played = Clock.nsecsElapsed() / 1000 * BytesPerSecond / 1000000;
			qint64 queued = PushedBytes - played;
			if (queued < 0){
				// アンダーランした場合は基準をリセット
				PushedBytes = played;
				queued = 0;
			}
			const qint64 deficit = (TargetBytes - queued) / BytesPerSample * BytesPerSample;
			// NOWAIT時などにデータが大量に溜まっても先行しすぎないよう上限を設ける
			// (GStreamer側のキューに溜まると遅延として残り続けるため)
			const qint64 limit = qMax<qint64>(0, (TargetBytes * 2 - queued) / BytesPerSample * BytesPerSample);
			bytesAvailable = qMin(qMax(bytesAvailable, deficit), limit);
		}
#endif
		return bytesAvailable;
	}

#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
	void resetClock(int rate, int latencyMs){
		BytesPerSecond = qint64(rate) * BytesPerSample;
		TargetBytes = BytesPerSecond * latencyMs / 1000 / BytesPerSample * BytesPerSample;
		PushedBytes = 0;
		Clock.start();
	}
#endif

protected:
	qint64 readData(char *data, qint64 maxlen) override
	{
		// オーディオコールバックを呼んでバッファにデータを取り込み
		CbFunc(CbData, reinterpret_cast<BYTE*>(data), maxlen);
#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
		PushedBytes += maxlen;
#endif
		return maxlen;
	}

	qint64 writeData(const char *data, qint64 len) override
	{
		// 読み取り専用なので常にエラーを返す
		return -1;
	}

private:
	CBF_SND CbFunc;
	void* CbData;
	int BytesPerSample;
#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
	QElapsedTimer Clock;
	qint64 BytesPerSecond = 0;
	qint64 TargetBytes = 0;
	mutable std::atomic<qint64> PushedBytes{0};
#endif
};


AudioOutputWrapper::AudioOutputWrapper(
	CBF_SND cbFunc,
	void *cbData,
	int rate,
	int samples,
	QObject *parent)
	: QObject(parent)
	, MediaDevices(new QMediaDevices(this))
	, ExpectedState(QAudio::StoppedState)
{
	Format.setChannelConfig(QAudioFormat::ChannelConfigMono);
	Format.setSampleRate(rate);
	Format.setSampleFormat(QAudioFormat::Int16);
	AudioBuffer = new AudioBufferWrapper(cbFunc, cbData, Format.bytesPerSample(), this);

	// バッファアンダーランを起こした場合に回復させる
	QTimer* recoveryTimer = new QTimer(this);
	connect(recoveryTimer, &QTimer::timeout, this, &AudioOutputWrapper::recoverPlayback);
	recoveryTimer->setInterval(1000);
	recoveryTimer->start();

#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
	// Qt 6.4以前のGStreamerバックエンドはプル方式でもreadyReadシグナルを契機に
	// データを読み取り、データが無いと待機状態のまま止まるため、定期的に通知する
	QTimer* readyReadTimer = new QTimer(this);
	readyReadTimer->setTimerType(Qt::PreciseTimer);
	connect(readyReadTimer, &QTimer::timeout, this, [this]{
		if (AudioBuffer && AudioBuffer->isOpen() && AudioBuffer->bytesAvailable() > 0){
			emit AudioBuffer->readyRead();
		}
	});
	readyReadTimer->setInterval(5);
	readyReadTimer->start();
#endif

	// サウンドデバイスの挿抜時に出力を切り替える
	connect(MediaDevices, &QMediaDevices::audioOutputsChanged, this, &AudioOutputWrapper::initDevice);

	initDevice();
}

AudioOutputWrapper::~AudioOutputWrapper()
{
}

void AudioOutputWrapper::start()
{
	AudioBuffer->open(QIODevice::ReadOnly | QIODevice::Unbuffered);
#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
	static_cast<AudioBufferWrapper*>(AudioBuffer.data())->resetClock(Format.sampleRate(), 60);
#endif
	if (!AudioSink.isNull()){
		AudioSink->stop(); // いったん止めたほうが安定する
		AudioSink->start(AudioBuffer);
	}
	ExpectedState = QAudio::ActiveState;
}


void AudioOutputWrapper::suspend()
{
	if (!AudioSink.isNull()){
		AudioSink->suspend();
	}
	ExpectedState = QAudio::SuspendedState;
}

void AudioOutputWrapper::resume()
{
	if (!AudioSink.isNull()){
		AudioSink->resume();
	}
	ExpectedState = QAudio::ActiveState;
}

void AudioOutputWrapper::stop()
{
	if (!AudioSink.isNull()){
		AudioSink->reset();
		AudioSink->stop();
		AudioBuffer->close();
	}
	ExpectedState = QAudio::StoppedState;
}

QAudio::State AudioOutputWrapper::state() const
{
	if (!AudioSink.isNull()){
		return AudioSink->state();
	} else {
		return QAudio::StoppedState;
	}
}

void AudioOutputWrapper::initDevice()
{
	auto device = QMediaDevices::defaultAudioOutput();
	auto deviceName = device.description();
	if (CurrentDevice == deviceName){
		// デバイス名が変更されていないのに呼ばれる場合があり、その場合は何もしない
		return;
	}
	qDebug() << "AudioOutputWrapper::initDevice deviceName:" << deviceName;

	// オブジェクトは作り直すがあるべき状態は呼び出し前の状態を維持する。
	auto state = ExpectedState;
	if (!AudioSink.isNull()){
		stop();
		AudioSink->deleteLater();
	}
	AudioSink = new QAudioSink(device, Format, this);
	qDebug()<< "AudioOutputWrapper::initDevice bufferSize:" <<AudioSink->bufferSize();
	ExpectedState = state;
	CurrentDevice = deviceName;
}

void AudioOutputWrapper::recoverPlayback()
{
	// バッファアンダーランなど、予期しないイベントによって
	// 内部で想定している状態と実際の状態に乖離が現れた場合
	// 状態を有るべき姿に復元を試みる。
	auto actualState = state();
#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
	// Qt 6.4以前のGStreamerバックエンドは供給が追いつくたびにIdleState(Underrun)になるが
	// 再生は継続しているため、再起動すると逆に音途切れの原因になる
	if (actualState == QAudio::IdleState && ExpectedState == QAudio::ActiveState){
		return;
	}
#endif
	if (actualState != ExpectedState){
		switch (ExpectedState){
		case QAudio::ActiveState:
		// 現状は、音が鳴るはずなのに鳴ってないという場合のみ
		// 再度再生状態に持っていく
		start();
		break;
		// それ以外の状態では何もしない。
		default:;
		}
	}
}
#endif // NOCALLBACK
#endif
