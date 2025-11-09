#include "audiooutputwrapper.h"
#include "qtel6.h"
#include "p6vxapp.h"

#ifndef NOSOUND
#include <QMediaDevices>
#include <QAudioSink>
#include <QBuffer>
#include <QMutex>
#include <QDebug>

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
		int bytesAvailable = qtel6->GetSoundReadySize() * BytesPerSample /
							 ((double)qtel6->GetSpeedRatio() / 100.0);
		return bytesAvailable;
	}

protected:
	qint64 readData(char *data, qint64 maxlen) override
	{
		// オーディオコールバックを呼んでバッファにデータを取り込み
		CbFunc(CbData, reinterpret_cast<BYTE*>(data), maxlen);
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

	initDevice();

	AudioBuffer = new AudioBufferWrapper(cbFunc, cbData, Format.bytesPerSample(), this);

	// バッファアンダーランを起こした場合に回復させる
	QTimer* recoveryTimer = new QTimer(this);
	connect(recoveryTimer, &QTimer::timeout, this, &AudioOutputWrapper::recoverPlayback);
	recoveryTimer->setInterval(1000);
	recoveryTimer->start();

	// サウンドデバイスの挿抜時に出力を切り替える
	connect(MediaDevices, &QMediaDevices::audioOutputsChanged, this, &AudioOutputWrapper::initDevice);
}

AudioOutputWrapper::~AudioOutputWrapper()
{
}

void AudioOutputWrapper::start()
{
	AudioBuffer->open(QIODevice::ReadOnly | QIODevice::Unbuffered);
	// バックエンド側のバッファサイズ
	// OSやバックエンド実装によって最適値が異なる。
	// デフォルト値で音の再生に支障がある場合は明示的に設定する。
#ifdef Q_OS_ANDROID
	AudioSink->setBufferSize(44100/30);
#endif
	AudioSink->stop(); // 一度stopしたほうが安定する
	AudioSink->start(AudioBuffer);
	ExpectedState = QAudio::ActiveState;
}


void AudioOutputWrapper::suspend()
{
	AudioSink->suspend();
	ExpectedState = QAudio::SuspendedState;
}

void AudioOutputWrapper::resume()
{
	AudioSink->resume();
	ExpectedState = QAudio::ActiveState;
}

void AudioOutputWrapper::stop()
{
	AudioSink->reset();
	AudioSink->stop();
	AudioBuffer->close();
	ExpectedState = QAudio::StoppedState;
}

QAudio::State AudioOutputWrapper::state() const
{
	return AudioSink->state();
}

void AudioOutputWrapper::initDevice()
{
	QAudioDevice device = QMediaDevices::defaultAudioOutput();
	if (!AudioSink.isNull()){
		AudioSink->deleteLater();
	}
	AudioSink = new QAudioSink(device, Format, this);
}

void AudioOutputWrapper::recoverPlayback()
{
	// バッファアンダーランなど、予期しないイベントによって
	// 内部で想定している状態と実際の状態に乖離が現れた場合
	// 状態を有るべき姿に復元を試みる。
	auto actualState = state();
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
