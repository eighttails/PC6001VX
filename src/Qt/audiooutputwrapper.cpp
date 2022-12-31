#include "audiooutputwrapper.h"

#ifndef NOSOUND
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
					   int sampleBytes,
					   QObject* parent)
		: QIODevice(parent)
		, CbFunc(cbFunc)
		, CbData(cbData)
		, SampleBytes(sampleBytes)
	{}

	virtual ~AudioBufferWrapper(){
		close();
	}

	bool isSequential() const override{
		return true;
	}

	qint64 size() const override
	{
		return SampleBytes;
	}

	qint64 bytesAvailable() const override{
		// 実際にエミュレータ側に溜まっているサンプル数を知る術はないが、
		// 適当な値を返しておかないとreadData()が呼ばれない。
		return SampleBytes;
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
	int SampleBytes;
};


AudioOutputWrapper::AudioOutputWrapper(const QAudioDevice &device,
									   const QAudioFormat &format,
									   CBF_SND cbFunc,
									   void *cbData,
									   int samples,
									   QObject *parent)
	: QObject(parent)
	, AudioSink(new QAudioSink(device, format, this))
{
	AudioBuffer = new AudioBufferWrapper(cbFunc, cbData, samples * format.bytesPerSample(), this);
}

AudioOutputWrapper::~AudioOutputWrapper()
{
}

void AudioOutputWrapper::start()
{
	AudioBuffer->open(QIODevice::ReadOnly | QIODevice::Unbuffered);
	// バックエンド側のバッファサイズ(適当かつ明示的に設定しておく)
	AudioSink->setBufferSize(44100);
	AudioSink->start(AudioBuffer);
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
	AudioBuffer->close();
}

int AudioOutputWrapper::bytesPerSample()
{
	return AudioSink->format().bytesPerSample();
}

QAudio::State AudioOutputWrapper::state() const
{
	return AudioSink->state();
}
#endif // NOCALLBACK
#endif
