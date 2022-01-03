#ifndef AUDIOOUTPUTWRAPPER_H
#define AUDIOOUTPUTWRAPPER_H
#ifndef NOSOUND
#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QAudioOutput>
#include "typedef.h"

// QAudioOutputのラッパークラス。
// P6Vのコードではサウンド制御が複数のスレッドから呼ばれるが、
// Qtの規約により実処理はオブジェクトの所属スレッドで実施する必要があるため
// サウンド制御の諸APIをinvokeMethod経由で呼べるようSLOTにする。
class AudioOutputWrapper : public QObject
{
	Q_OBJECT
public:
	explicit AudioOutputWrapper(const QAudioDeviceInfo& info,
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

protected slots:
	void pullAudioData();
private:
	QAudioOutput* AudioOutput;
	QPointer<QIODevice> AudioBuffer;
	CBF_SND CbFunc;
	void* CbData;
	QTimer PollingTimer;
};
#endif // NOSOUND
#endif // AUDIOOUTPUTWRAPPER_H
