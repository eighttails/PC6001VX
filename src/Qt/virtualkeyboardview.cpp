#include "virtualkeyboardview.h"

#include "../osd.h"
#include "keystatewatcher.h"
#include "virtualkeyboardscene.h"

#include <QEvent>
#include <QQmlContext>
#include <QQmlError>

VirtualKeyboardView::VirtualKeyboardView(QWidget *parent)
	: QQuickWidget(parent)
{
	setAttribute(Qt::WA_Hover);
	setClearColor(Qt::black);
	setResizeMode(QQuickWidget::SizeRootObjectToView);
	rootContext()->setContextProperty(QStringLiteral("keyboard"), this);
	setSource(QUrl(QStringLiteral("qrc:/qml/VirtualKeyboard.qml")));
	if (status() == QQuickWidget::Error) {
		for (const QQmlError &error : errors()) {
			qWarning() << error;
		}
	}
}

void VirtualKeyboardView::setKeyboardScene(VirtualKeyboardScene *scene)
{
	if (Scene == scene) return;

	if (Scene) {
		disconnect(Scene, nullptr, this, nullptr);
	}

	Scene = scene;
	if (Scene) {
		connect(Scene, &VirtualKeyboardScene::itemsChanged, this, &VirtualKeyboardView::sceneChanged);
		connect(Scene, &VirtualKeyboardScene::sceneRectChanged, this, &VirtualKeyboardView::sceneChanged);
	}
	emit sceneChanged();
}

void VirtualKeyboardView::setKeyStateWatcher(KeyStateWatcher *watcher)
{
	if (Scene) {
		Scene->setKeyStateWatcher(watcher);
	}
}

QVariantList VirtualKeyboardView::items() const
{
	return Scene ? Scene->items() : QVariantList();
}

qreal VirtualKeyboardView::sceneWidth() const
{
	return Scene ? Scene->sceneWidth() : 0;
}

qreal VirtualKeyboardView::sceneHeight() const
{
	return Scene ? Scene->sceneHeight() : 0;
}

bool VirtualKeyboardView::hasHeightForWidth() const
{
	return true;
}

int VirtualKeyboardView::heightForWidth(int width) const
{
	if(sceneWidth() > 0){
		auto sceneAspectRatio = sceneHeight() / sceneWidth();
		return int(sceneAspectRatio * width);
	} else {
		return width;
	}
}

bool VirtualKeyboardView::event(QEvent *event)
{
	switch (event->type()) {
	case QEvent::Enter:
	case QEvent::HoverMove:
		OSD_ShowCursor(true);
		break;
	case QEvent::Leave:
		OSD_ShowCursor(true);
		break;
	default:;
	}
	return QQuickWidget::event(event);
}
