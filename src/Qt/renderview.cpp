#ifndef NOOPENGL
#include <QtOpenGL>
#endif
#include <QtWidgets>
#include <QSettings>
#include "renderview.h"
#include "qtp6vxapplication.h"


RenderView::RenderView(QGraphicsScene* scene, QWidget *parent)
	: QGraphicsView(scene, parent)
{
    setSizeIncrement(1, 1);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setBackgroundBrush(Qt::black);
    setStyleSheet( "QGraphicsView { border-style: none; }" );

#ifndef NOOPENGL
	if(QtP6VXApplication::getSetting(QtP6VXApplication::keyHwAccel).toBool()){
		QGLWidget* glw = new QGLWidget(this);
		// QGraphicsViewに使うにはOpenGL2以上が必要
		if(glw->format().majorVersion() >= 2){
			setViewport(glw);
			setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
		} else {
			glw->deleteLater();
		}
	}
#endif

#ifdef WIN32
    //IMEを無効化
    ImmAssociateContext( (HWND)winId(), NULL );
#endif

	// ウィンドウ位置とサイズを復元
	restoreGeometry(QtP6VXApplication::getSetting(QtP6VXApplication::keyGeometry).toByteArray());
	if(QtP6VXApplication::getSetting(QtP6VXApplication::keyMaximized).toBool()){
		showMaximized();
	}
}

RenderView::~RenderView()
{
}

void RenderView::fitContent()
{
	const bool fixMag = QtP6VXApplication::getSetting(QtP6VXApplication::keyFixMagnification).toBool();
	//ウィンドウ全体に表示されるように表示倍率を調整
	qreal scaleRatio = fixMag
			? QtP6VXApplication::getSetting(QtP6VXApplication::keyMagnification).toReal()
			: qMin(width() / scene()->width(), height() / scene()->height());
	resetTransform();
	scale(scaleRatio, scaleRatio);

	//表示倍率固定の場合は中心に配置
	if (fixMag){
		translate((width() - scene()->width()) / 2, (height() - scene()->height()) / 2);
	}else {
		QtP6VXApplication::setSetting(QtP6VXApplication::keyMagnification, scaleRatio);
	}
}

void RenderView::resizeWindowByRatio(int ratio)
{
	qreal r = double(ratio) / 100;
	// 最大化、フルスクリーン中は倍率固定モードにする。
	if((windowState() & Qt::WindowMaximized) || (windowState() & Qt::WindowFullScreen)){
		QtP6VXApplication::setSetting(QtP6VXApplication::keyFixMagnification, true);
		QtP6VXApplication::setSetting(QtP6VXApplication::keyMagnification, r);
		return;
	} else {
		QtP6VXApplication::setSetting(QtP6VXApplication::keyFixMagnification, false);
	}
	setGeometry(x(), y(), scene()->width() * r, scene()->height() * r);
}

void RenderView::paintEvent(QPaintEvent *event)
{
    QtP6VXApplication* app = qobject_cast<QtP6VXApplication*>(qApp);
    if(isActiveWindow()){
        fitContent();
        if(app->isTiltEnabled()){
            //TILTモードの回転
			const qreal unit = 0.5; //0.5度単位で回転
			rotate(unit * app->getTiltStep());
        }
    }
	QGraphicsView::paintEvent(event);
}

void RenderView::closeEvent(QCloseEvent *event)
{
	// ウィンドウ位置とサイズを保存
	QtP6VXApplication::setSetting(QtP6VXApplication::keyGeometry, saveGeometry());
	QtP6VXApplication::setSetting(QtP6VXApplication::keyMaximized, isMaximized());
	QGraphicsView::closeEvent(event);
}

