#ifndef KEYSTATEWATCHER_H
#define KEYSTATEWATCHER_H

#include <QObject>
#include <QMutex>
#include <memory>

class KEY6;
class JOY6;

//キーボードの状態を監視し、仮想キーボードに通知する
class KeyStateWatcher : public QObject
{
	Q_OBJECT
public:
	explicit KeyStateWatcher(KEY6* key, JOY6* joy, QObject *parent = nullptr);

signals:
	void stateChanged(
			bool ON_SHIFT,	// SHIFT
			bool ON_CTRL,	// CTRL
			bool ON_GRAPH,	// GRAPH
			bool ON_KANA,	// かな
			bool ON_KKANA,	// カタカナ
			bool ON_CAPS,	// CAPS
			bool ON_ROMAJI	// ローマ字入力
			);

public:
	enum Modifiers {
		SHIFT	= 0x01,
		CTRL	= 0x02,
		GRAPH	= 0x04,
		KANA	= 0x08,
		KKANA	= 0x16,
		CAPS	= 0x32,
		ROMAJI	= 0x64,
	};
	int GetModifierStatus();


protected slots:
	// キーボードの状態をポーリング
	void poll();

protected:
	KEY6* Key;	// 監視対象のキーボード
	JOY6* Joy;	// 監視対象のジョイスティック

	bool ON_SHIFT;	// SHIFT
	bool ON_CTRL;	// CTRL
	bool ON_GRAPH;	// GRAPH
	bool ON_KANA;	// かな
	bool ON_KKANA;	// カタカナ
	bool ON_CAPS;	// CAPS
	bool ON_ROMAJI;	// ローマ字入力

	QMutex mutex;
};

#endif // KEYSTATEWATCHER_H
