#include "audiooutputwrapper.h"
#include "p6vxapp.h"

#ifndef NOSOUND
AudioOutputWrapper::AudioOutputWrapper(const QAudioDeviceInfo &info,
									   const QAudioFormat &format,
									   CBF_SND cbFunc,
									   void *cbData,
									   int samples,
									   QObject *parent)
	: QObject(parent)
	, AudioOutput(new QAudioOutput(info, format, this))
	, CbFunc(cbFunc)
	, CbData(cbData)
{
	// サンプルサイズに関わらず0.5秒分のバッファを確保
	// バッファが大きすぎると遅延が大きくなる
	AudioOutput->setBufferSize(44100 * 0.5 * sizeof(uint16_t));
	// 1フレーム(16.6ms)より短い間隔でポーリング
	// ポーリング間隔が短すぎると処理が重くなり、
	// 長すぎるとエミュレータ側のバッファが滞留して音飛びの原因になる。
	PollingTimer.setInterval(10);
	connect(&PollingTimer, SIGNAL(timeout()), this, SLOT(pullAudioData()));
	PollingTimer.start();
}

AudioOutputWrapper::~AudioOutputWrapper()
{
	PollingTimer.stop();
}

void AudioOutputWrapper::start()
{
	AudioBuffer	= AudioOutput->start();
	PollingTimer.start();
}

void AudioOutputWrapper::suspend()
{
	AudioOutput->suspend();
	PollingTimer.stop();
}

void AudioOutputWrapper::resume()
{
	PollingTimer.start();
	AudioOutput->resume();
}

void AudioOutputWrapper::stop()
{
	AudioOutput->stop();
	PollingTimer.stop();
}

QAudio::State AudioOutputWrapper::state() const
{
	return AudioOutput->state();
}

void AudioOutputWrapper::pullAudioData()
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	// テンポラリバッファを確保
	size_t bufSize = AudioOutput->bytesFree();
	BYTE stream[bufSize];
	// オーディオコールバックを呼んでバッファにデータを取り込み
	CbFunc(CbData, stream, bufSize);

	// 実際のオーディオバッファに書き込み(発音)
	if(!app->isAVI()){
		if(AudioBuffer){
			AudioBuffer->write(reinterpret_cast<const char*>(stream), bufSize);
		}
	}
}
#endif
