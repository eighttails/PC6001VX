#ifndef AUDIOOUTPUTWRAPPER_H
#define AUDIOOUTPUTWRAPPER_H
#ifndef NOSOUND
#include <QObject>
#include <QPointer>
#include <QAudioOutput>

// QAudioOutputのラッパークラス。
// P6Vのコードではサウンド制御が複数のスレッドから呼ばれるが、
// Qtの規約により実処理はオブジェクトの所属スレッドで実施する必要があるため
// サウンド制御の諸APIをinvokeMethod経由で呼べるようSLOTにする。
class AudioOutputWrapper : public QObject
{
	Q_OBJECT
public:
	explicit AudioOutputWrapper(const QAudioDeviceInfo& info,
								const QAudioFormat& format, QObject* parent = nullptr);

public slots:
	// 下記のメソッドはinvokeMethodから呼ぶこと。
	QPointer<QIODevice> start();
	void suspend();
	void resume();
	void stop();

public:
	QAudio::State state() const;

private:
	QAudioOutput* AudioOutput;
};
#endif // NOSOUND
#endif // AUDIOOUTPUTWRAPPER_H
