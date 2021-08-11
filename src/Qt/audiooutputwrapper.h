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
								const QAudioFormat& format, QObject* parent = nullptr)
		: QObject(parent)
		, AudioOutput(new QAudioOutput(info, format, this)) {}

public slots:
	// 下記のメソッドはinvokeMethodから呼ぶこと。
	QPointer<QIODevice> start() { return AudioOutput->start(); }
	void suspend() { AudioOutput->suspend(); }
	void resume() { AudioOutput->resume(); }
	void stop() { AudioOutput->stop(); }

public:
	QAudio::State state() const { return AudioOutput->state(); }

private:
	QAudioOutput* AudioOutput;
};
#endif // NOSOUND
#endif // AUDIOOUTPUTWRAPPER_H
