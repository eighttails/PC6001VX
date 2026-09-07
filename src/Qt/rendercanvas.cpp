#include "rendercanvas.h"

#include <QPainter>
#include <QQuickWindow>
#include <QSGSimpleTextureNode>

#include <algorithm>

RenderCanvas::RenderCanvas(QQuickItem *parent)
	: QQuickItem(parent)
{
	setFlag(ItemHasContents);
}

int RenderCanvas::sceneWidth() const
{
	return SceneWidth;
}

void RenderCanvas::setSceneWidth(int width)
{
	if (SceneWidth == width) return;

	SceneWidth = width;
	updatePaintBounds();
	emit sceneSizeChanged();
	update();
}

int RenderCanvas::sceneHeight() const
{
	return SceneHeight;
}

void RenderCanvas::setSceneHeight(int height)
{
	if (SceneHeight == height) return;

	SceneHeight = height;
	updatePaintBounds();
	emit sceneSizeChanged();
	update();
}

qreal RenderCanvas::paintLeft() const
{
	return PaintBounds.left();
}

qreal RenderCanvas::paintTop() const
{
	return PaintBounds.top();
}

qreal RenderCanvas::paintWidth() const
{
	return PaintBounds.width();
}

qreal RenderCanvas::paintHeight() const
{
	return PaintBounds.height();
}

void RenderCanvas::addOrUpdateLayer(int x, int y, double scaleX, double scaleY, const QImage &image, bool smooth, qreal z)
{
	for (Layer &layer : Layers) {
		if (layer.x == x && layer.y == y && qFuzzyCompare(layer.z, z)) {
			layer.scaleX = scaleX;
			layer.scaleY = scaleY;
			layer.image = image;
			layer.smooth = smooth;
			updatePaintBounds();
			update();
			return;
		}
	}

	Layer layer;
	layer.x = x;
	layer.y = y;
	layer.scaleX = scaleX;
	layer.scaleY = scaleY;
	layer.image = image;
	layer.smooth = smooth;
	layer.z = z;
	layer.order = NextOrder++;
	Layers.push_back(layer);
	updatePaintBounds();
	update();
}

void RenderCanvas::clearLayers()
{
	Layers.clear();
	NextOrder = 0;
	updatePaintBounds();
	update();
}

bool RenderCanvas::isFilteringAt(int x, int y) const
{
	for (const Layer &layer : Layers) {
		const QRectF bounds(layer.x, layer.y, layer.image.width() * layer.scaleX, layer.image.height() * layer.scaleY);
		if (qFuzzyIsNull(layer.z) && bounds.contains(QPointF(x, y))) {
			return layer.smooth;
		}
	}
	return false;
}

QImage RenderCanvas::renderToImage(const QRect &rect) const
{
	QImage image(qMax(SceneWidth, rect.right() + 1), qMax(SceneHeight, rect.bottom() + 1), QImage::Format_RGB888);
	image.fill(Qt::black);

	QPainter painter(&image);
	paintLayers(&painter);
	painter.end();

	return image.copy(rect);
}

QSGNode *RenderCanvas::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
	delete oldNode;

	auto *rootNode = new QSGNode;
	QVector<const Layer *> sorted;
	sorted.reserve(Layers.size());
	for (const Layer &layer : Layers) {
		if (!layer.image.isNull()) {
			sorted.push_back(&layer);
		}
	}
	std::stable_sort(sorted.begin(), sorted.end(), [](const Layer *a, const Layer *b) {
		if (!qFuzzyCompare(a->z, b->z)) return a->z < b->z;
		return a->order < b->order;
	});

	for (const Layer *layer : sorted) {
		auto *textureNode = new QSGSimpleTextureNode;
		textureNode->setTexture(window()->createTextureFromImage(layer->image));
		textureNode->setOwnsTexture(true);
		textureNode->setFiltering(layer->smooth ? QSGTexture::Linear : QSGTexture::Nearest);
		textureNode->setRect(layer->x - PaintBounds.left(),
							 layer->y - PaintBounds.top(),
							 layer->image.width() * layer->scaleX,
							 layer->image.height() * layer->scaleY);
		rootNode->appendChildNode(textureNode);
	}

	return rootNode;
}

void RenderCanvas::updatePaintBounds()
{
	QRectF bounds(0, 0, SceneWidth, SceneHeight);

	for (const Layer &layer : Layers) {
		if (layer.image.isNull()) continue;

		const QRectF layerBounds(layer.x,
								 layer.y,
								 layer.image.width() * layer.scaleX,
								 layer.image.height() * layer.scaleY);
		bounds = bounds.united(layerBounds.normalized());
	}

	if (PaintBounds == bounds) return;

	PaintBounds = bounds;
	emit paintBoundsChanged();
}

void RenderCanvas::paintLayers(QPainter *painter, const QPointF &offset) const
{
	QVector<Layer> sorted = Layers;
	std::stable_sort(sorted.begin(), sorted.end(), [](const Layer &a, const Layer &b) {
		if (!qFuzzyCompare(a.z, b.z)) return a.z < b.z;
		return a.order < b.order;
	});

	for (const Layer &layer : sorted) {
		if (layer.image.isNull()) continue;

		painter->save();
		painter->setRenderHint(QPainter::SmoothPixmapTransform, layer.smooth);
		painter->translate(offset);
		painter->translate(layer.x, layer.y);
		painter->scale(layer.scaleX, layer.scaleY);
		painter->drawImage(QPointF(0, 0), layer.image);
		painter->restore();
	}
}
