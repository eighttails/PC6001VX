#ifndef VIRTUALKEYBOARDVIEW_H
#define VIRTUALKEYBOARDVIEW_H

#include <QPointer>
#include <QQuickWidget>

class KeyStateWatcher;
class VirtualKeyboardScene;

class VirtualKeyboardView : public QQuickWidget
{
	Q_OBJECT
	Q_PROPERTY(QVariantList items READ items NOTIFY sceneChanged)
	Q_PROPERTY(qreal sceneWidth READ sceneWidth NOTIFY sceneChanged)
	Q_PROPERTY(qreal sceneHeight READ sceneHeight NOTIFY sceneChanged)
public:
	VirtualKeyboardView(QWidget *parent = Q_NULLPTR);

	void setKeyboardScene(VirtualKeyboardScene *scene);
	void setKeyStateWatcher(KeyStateWatcher *watcher);

	QVariantList items() const;
	qreal sceneWidth() const;
	qreal sceneHeight() const;

signals:
	void sceneChanged();

public slots:

	// QWidget interface
public:
	virtual bool hasHeightForWidth() const override;
	virtual int heightForWidth(int width) const override;

	// QWidget interface
protected:
	virtual bool event(QEvent *event) override;

private:
	QPointer<VirtualKeyboardScene> Scene;
};

#endif // VIRTUALKEYBOARDVIEW_H
