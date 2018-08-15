#ifndef KEYSTATEWATCHER_H
#define KEYSTATEWATCHER_H

#include <QObject>

class KEY6;

//キーボードの状態を監視し、仮想キーボードに通知する
class KeyStateWatcher : public QObject
{
	Q_OBJECT
public:
	explicit KeyStateWatcher(KEY6* key, QObject *parent = nullptr);

signals:
	void stateChanged(
			bool ON_SHIFT,	// SHIFT
			bool ON_GRAPH,	// GRAPH
			bool ON_KANA,	// かな
			bool ON_KKANA,	// カタカナ
			bool ON_CAPS	// CAPS
			);

public slots:

protected slots:
	// キーボードの状態をポーリング
	void poll();

protected:
	KEY6* Key;	//監視対象のキーボード

	bool ON_SHIFT;	// SHIFT
	bool ON_GRAPH;	// GRAPH
	bool ON_KANA;	// かな
	bool ON_KKANA;	// カタカナ
	bool ON_CAPS;	// CAPS
};

#endif // KEYSTATEWATCHER_H
