#ifndef RENDERVIEW_H
#define RENDERVIEW_H

#include <QGraphicsView>

class RenderView : public QGraphicsView
{
	Q_OBJECT
public:
	explicit RenderView(QGraphicsScene *scene, QWidget *parent = 0);
	~RenderView();
signals:

public slots:
	//シーングラフ全体がウィジェットに収まるように表示倍率を調整
	//fitInViewだと微妙に倍率が合わないので独自実装
	void fitContent();

protected:
	virtual void paintEvent(QPaintEvent* event);
	virtual void closeEvent(QCloseEvent* event);

};

#endif // RENDERVIEW_H
