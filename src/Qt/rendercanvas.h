#ifndef RENDERCANVAS_H
#define RENDERCANVAS_H

#include <QImage>
#include <QPointF>
#include <QQuickPaintedItem>
#include <QRect>
#include <QRectF>
#include <QVector>

class RenderCanvas : public QQuickPaintedItem
{
	Q_OBJECT
	Q_PROPERTY(int sceneWidth READ sceneWidth WRITE setSceneWidth NOTIFY sceneSizeChanged)
	Q_PROPERTY(int sceneHeight READ sceneHeight WRITE setSceneHeight NOTIFY sceneSizeChanged)
	Q_PROPERTY(qreal paintLeft READ paintLeft NOTIFY paintBoundsChanged)
	Q_PROPERTY(qreal paintTop READ paintTop NOTIFY paintBoundsChanged)
	Q_PROPERTY(qreal paintWidth READ paintWidth NOTIFY paintBoundsChanged)
	Q_PROPERTY(qreal paintHeight READ paintHeight NOTIFY paintBoundsChanged)
public:
	explicit RenderCanvas(QQuickItem *parent = nullptr);

	int sceneWidth() const;
	void setSceneWidth(int width);

	int sceneHeight() const;
	void setSceneHeight(int height);

	qreal paintLeft() const;
	qreal paintTop() const;
	qreal paintWidth() const;
	qreal paintHeight() const;

	void addOrUpdateLayer(int x, int y, double scaleX, double scaleY, const QImage &image, bool smooth, qreal z = 0.0);
	void clearLayers();
	bool isFilteringAt(int x, int y) const;
	QImage renderToImage(const QRect &rect) const;

	void paint(QPainter *painter) override;

signals:
	void sceneSizeChanged();
	void paintBoundsChanged();

private:
	struct Layer {
		int x = 0;
		int y = 0;
		double scaleX = 1.0;
		double scaleY = 1.0;
		QImage image;
		bool smooth = false;
		qreal z = 0.0;
		int order = 0;
	};

	void updatePaintBounds();
	void paintLayers(QPainter *painter, const QPointF &offset = QPointF()) const;

	int SceneWidth = 0;
	int SceneHeight = 0;
	QRectF PaintBounds;
	int NextOrder = 0;
	QVector<Layer> Layers;
};

#endif // RENDERCANVAS_H
