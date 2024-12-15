#ifndef AUDIOOUTPUTWRAPPER_H
#define AUDIOOUTPUTWRAPPER_H
#ifndef NOSOUND
#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QAudioDevice>
#include <QAudioFormat>
#include "typedef.h"

class QAudioSink;
#ifdef NOCALLBACK
// コールバックを使わずにAudioSinkにサンプルバッファをPUSHする実装
// #TODO 現状では、一度バッファアンダーランを起こすと
// それ以降音が出なくなる問題がある。

class AudioBufferWrapper;

// QAudioOutputのラッパークラス。
// P6Vのコードではサウンド制御が複数のスレッドから呼ばれるが、
// Qtの規約により実処理はオブジェクトの所属スレッドで実施する必要があるため
// サウンド制御の諸APIをinvokeMethod経由で呼べるようSLOTにする。
class AudioOutputWrapper : public QObject
{
	Q_OBJECT
public:
	explicit AudioOutputWrapper(const QAudioDevice& device,
								const QAudioFormat& format,
								CBF_SND cbFunc,
								void *cbData,
								int samples,
								QObject* parent = nullptr);
	virtual ~AudioOutputWrapper();

public slots:
	// 下記のメソッドはinvokeMethodから呼ぶこと。
	void start();
	void suspend();
	void resume();
	void stop();
	void writeAudioStream(BYTE *stream, int samples);
	int queuedAudioSamples();
	int bytesPerSample();
public:
	QAudio::State state() const;

private:
	QAudioSink* AudioSink;
	QPointer<QIODevice> AudioBuffer;
	int Samples;

};
#else


// コールバックを受けてにAudioSinkからサンプルバッファをPULLされる実装

// QAudioOutputのラッパークラス。
// P6Vのコードではサウンド制御が複数のスレッドから呼ばれるが、
// Qtの規約により実処理はオブジェクトの所属スレッドで実施する必要があるため
// サウンド制御の諸APIをinvokeMethod経由で呼べるようSLOTにする。
class AudioOutputWrapper : public QObject
{
	Q_OBJECT
public:
	explicit AudioOutputWrapper(const QAudioDevice& device,
								const QAudioFormat& format,
								CBF_SND cbFunc,
								void* cbData,
								int samples,
								QObject* parent = nullptr);
	virtual ~AudioOutputWrapper();

public slots:
	// 下記のメソッドはinvokeMethodから呼ぶこと。
	void start();
	void suspend();
	void resume();
	void stop();
public:
	QAudio::State state() const;

private slots:
	void recoverPlayback();
private:
	QAudioSink* AudioSink;
	QPointer<QIODevice> AudioBuffer;
	QAudio::State ExpectedState;
};
#endif // NOCALLBACK
#endif // NOSOUND
#endif // AUDIOOUTPUTWRAPPER_H
