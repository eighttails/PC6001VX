#ifndef RENDERVIEW_H
#define RENDERVIEW_H

#include <QImage>
#include <QQuickWidget>
#include <QRect>
#include <QSize>

class RenderCanvas;

class RenderView : public QQuickWidget
{
	Q_OBJECT
public:
	explicit RenderView(QWidget *parent = 0);
	~RenderView();

	int sceneWidth() const;
	int sceneHeight() const;
	void setSceneSize(int width, int height);
	void layoutBitmap(int x, int y, double scaleX, double scaleY, const QImage &image, bool smooth, qreal z = 0.0);
	void clearLayout();
	bool isFilteringAt(int x, int y) const;
	QImage renderSceneImage(const QRect &rect) const;

signals:
	void resized(QSize);

public slots:
	// シーングラフ全体がウィジェットに収まるように表示倍率を調整
	// fitInViewだと微妙に倍率が合わないので独自実装
	void fitContent();

	// 指定した倍率(%)で表示されるようにウィンドウをリサイズ
	void resizeWindowByRatio(int ratio);

protected slots:
	void initializeSize();
protected:
	// QWidget interface
	virtual bool event(QEvent *) override;
	virtual void paintEvent(QPaintEvent* event) override;
	virtual void contextMenuEvent(QContextMenuEvent *event) override;
	virtual void wheelEvent(QWheelEvent *event) override;
	virtual void mouseReleaseEvent(QMouseEvent *event) override;
	virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
	virtual void dragEnterEvent(QDragEnterEvent *event) override;
	virtual void dragMoveEvent(QDragMoveEvent *event) override;
	virtual void dropEvent(QDropEvent *event) override;

private:
	void updateRootProperties();

	QSize SceneSize;
	RenderCanvas *Canvas;
};

#endif // RENDERVIEW_H
