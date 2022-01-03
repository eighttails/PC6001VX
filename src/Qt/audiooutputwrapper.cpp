#include "audiooutputwrapper.h"

AudioOutputWrapper::AudioOutputWrapper(const QAudioDeviceInfo &info,
									   const QAudioFormat &format,
									   QObject *parent)
	: QObject(parent)
	, AudioOutput(new QAudioOutput(info, format, this))
{

}

QPointer<QIODevice> AudioOutputWrapper::start()
{
	return AudioOutput->start();
}

void AudioOutputWrapper::suspend()
{
	AudioOutput->suspend();
}

void AudioOutputWrapper::resume()
{
	AudioOutput->resume();
}

void AudioOutputWrapper::stop()
{
	AudioOutput->stop();
}

QAudio::State AudioOutputWrapper::state() const
{
	return AudioOutput->state();
}
