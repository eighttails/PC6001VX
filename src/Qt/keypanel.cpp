#include <QVBoxLayout>
#include <QShowEvent>
#include "keypanel.h"
#include "keypanelbutton.h"

#include "p6vxapp.h"

KeyPanel::KeyPanel(QWidget *parent)
	: QWidget(parent)
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	setWindowFlags(Qt::Tool);
	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	setMaximumSize(1,1);

	QVBoxLayout* l = new QVBoxLayout();
	setLayout(l);
	l->setMargin(0);
	l->setSpacing(0);
	l->addWidget(new KeyPanelButton(this, tr("F1"), KVC_F1));
	l->addWidget(new KeyPanelButton(this, tr("F2"), KVC_F2));
	l->addWidget(new KeyPanelButton(this, tr("F3"), KVC_F3));
	l->addWidget(new KeyPanelButton(this, tr("F4"), KVC_F4));
	l->addWidget(new KeyPanelButton(this, tr("F5"), KVC_F5));
	l->addWidget(new KeyPanelButton(this, tr("かな"), KVC_HIRAGANA));
	l->addWidget(new KeyPanelButton(this, tr("^ / へ"), KVC_CARET));
	l->addWidget(new KeyPanelButton(this, tr("\\ / ろ"), KVC_UNDERSCORE));
	l->addWidget(new KeyPanelButton(this, tr("] / む"), KVC_RBRACKET));
	l->addWidget(new KeyPanelButton(this, tr("MODE"), KVC_PAGEDOWN));
	l->addWidget(new KeyPanelButton(this, tr("PAGE"), KVC_PAGEUP));
	l->addSpacing(10);
	l->addWidget(new KeyPanelButton(this, tr("SAVE"), KVC_MUHENKAN));
	l->addSpacing(10);
	l->addWidget(new KeyPanelButton(this, tr("LOAD"), KVC_HENKAN));

	adjustSize();
	QPoint p = app->getSetting(P6VXApp::keyKeyPanelPosition).toPoint();
	move(p);
	if(app->getSetting(P6VXApp::keyKeyPanelVisible).toBool()){
		show();
	}
}

void KeyPanel::moveEvent(QMoveEvent *)
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	app->setSetting(P6VXApp::keyKeyPanelPosition, pos());
}


void KeyPanel::showEvent(QShowEvent *)
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	app->setSetting(P6VXApp::keyKeyPanelVisible, true);
}

void KeyPanel::closeEvent(QCloseEvent *)
{
	P6VXApp* app = qobject_cast<P6VXApp*>(qApp);
	app->setSetting(P6VXApp::keyKeyPanelVisible, false);
}
