#include "audiooutputwrapper.h"
#include "p6vxapp.h"

#ifndef NOSOUND
#include <QAudioSink>
#include <QBuffer>


class AudioBufferWrapper : public QIODevice
{
public:
	AudioBufferWrapper(CBF_SND cbFunc,
					   void *cbData,
					   QObject* parent)
		: QIODevice(parent)
		, CbFunc(cbFunc)
		, CbData(cbData)
	{}

	virtual ~AudioBufferWrapper(){
		close();
	}

	qint64 bytesAvailable() const override
	{
		return 1024;
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
	AudioBuffer = new AudioBufferWrapper(cbFunc, cbData, this);
}

AudioOutputWrapper::~AudioOutputWrapper()
{
}

void AudioOutputWrapper::start()
{
	AudioBuffer->open(QIODevice::ReadOnly | QIODevice::Unbuffered);
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

QAudio::State AudioOutputWrapper::state() const
{
	return AudioSink->state();
}

#endif
