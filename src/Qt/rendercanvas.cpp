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
	QMutexLocker lock(&LayersMutex);
	return SceneWidth;
}

void RenderCanvas::setSceneWidth(int width)
{
	{
		QMutexLocker lock(&LayersMutex);
		if (SceneWidth == width) return;

		SceneWidth = width;
		updatePaintBounds();
	}
	emit sceneSizeChanged();
	update();
}

int RenderCanvas::sceneHeight() const
{
	QMutexLocker lock(&LayersMutex);
	return SceneHeight;
}

void RenderCanvas::setSceneHeight(int height)
{
	{
		QMutexLocker lock(&LayersMutex);
		if (SceneHeight == height) return;

		SceneHeight = height;
		updatePaintBounds();
	}
	emit sceneSizeChanged();
	update();
}

qreal RenderCanvas::paintLeft() const
{
	QMutexLocker lock(&LayersMutex);
	return PaintBounds.left();
}

qreal RenderCanvas::paintTop() const
{
	QMutexLocker lock(&LayersMutex);
	return PaintBounds.top();
}

qreal RenderCanvas::paintWidth() const
{
	QMutexLocker lock(&LayersMutex);
	return PaintBounds.width();
}

qreal RenderCanvas::paintHeight() const
{
	QMutexLocker lock(&LayersMutex);
	return PaintBounds.height();
}

void RenderCanvas::addOrUpdateLayer(int x, int y, double scaleX, double scaleY, const QImage &image, bool smooth, qreal z)
{
	{
		QMutexLocker lock(&LayersMutex);

		bool updated = false;
		for (Layer &layer : Layers) {
			if (layer.x == x && layer.y == y && qFuzzyCompare(layer.z, z)) {
				layer.scaleX = scaleX;
				layer.scaleY = scaleY;
				layer.image = image;
				layer.smooth = smooth;
				updated = true;
				break;
			}
		}

		if (!updated) {
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
		}

		updatePaintBounds();
	}
	update();
}

void RenderCanvas::clearLayers()
{
	{
		QMutexLocker lock(&LayersMutex);
		Layers.clear();
		NextOrder = 0;
		updatePaintBounds();
	}
	update();
}

bool RenderCanvas::isFilteringAt(int x, int y) const
{
	QMutexLocker lock(&LayersMutex);
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
	QMutexLocker lock(&LayersMutex);

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

	QQuickWindow *win = window();
	if (!win) {
		// シーングラフが利用できない場合は描画しない
		// (フォルダ選択ダイアログ表示中など、レンダリングコンテキストが失われている場合)
		return nullptr;
	}

	QMutexLocker lock(&LayersMutex);

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

	bool textureCreationFailed = false;
	for (const Layer *layer : sorted) {
		QSGTexture *texture = win->createTextureFromImage(layer->image);
		if (!texture) {
			// グラフィックスコンテキストが無効な状態ではテクスチャを生成できない。
			// nullptrのままノードを追加するとレンダラーがクラッシュするためスキップする
			textureCreationFailed = true;
			continue;
		}

		auto *textureNode = new QSGSimpleTextureNode;
		textureNode->setTexture(texture);
		textureNode->setOwnsTexture(true);
		textureNode->setFiltering(layer->smooth ? QSGTexture::Linear : QSGTexture::Nearest);
		textureNode->setRect(layer->x - PaintBounds.left(),
							 layer->y - PaintBounds.top(),
							 layer->image.width() * layer->scaleX,
							 layer->image.height() * layer->scaleY);
		rootNode->appendChildNode(textureNode);
	}

	if (textureCreationFailed) {
		// コンテキストが復帰したら描画し直す
		update();
	}

	return rootNode;
}

void RenderCanvas::itemChange(ItemChange change, const ItemChangeData &value)
{
	// ウィンドウから切り離される、あるいは再接続される際に描画内容を作り直す
	if (change == ItemSceneChange) {
		update();
	}

	QQuickItem::itemChange(change, value);
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
	// シグナルはミューテックスを保持したまま送出されるため、
	// 接続先(QMLのプロパティバインディング)からは同一スレッドで
	// 再帰的にゲッターが呼ばれないよう注意すること
	QMetaObject::invokeMethod(this, &RenderCanvas::paintBoundsChanged, Qt::QueuedConnection);
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
