#include <QVBoxLayout>
#include <QShowEvent>
#include "keypanel.h"
#include "keypanelbutton.h"

#include "qtp6vxapplication.h"

KeyPanel::KeyPanel(QWidget *parent)
	: QWidget(parent)
{
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
	l->addWidget(new KeyPanelButton(this, tr("MODE"), KVC_PAGEDOWN));
	l->addWidget(new KeyPanelButton(this, tr("PAGE"), KVC_PAGEUP));
	l->addSpacing(10);
	l->addWidget(new KeyPanelButton(this, tr("SAVE"), KVC_MUHENKAN));
	l->addSpacing(10);
	l->addWidget(new KeyPanelButton(this, tr("LOAD"), KVC_HENKAN));

	adjustSize();
	QPoint p = QtP6VXApplication::getSetting(QtP6VXApplication::keyKeyPanelPosition).toPoint();
	setGeometry(p.x(), p.y(), width(), height());
	if(QtP6VXApplication::getSetting(QtP6VXApplication::keyKeyPanelVisible).toBool()){
		show();
	}
}

void KeyPanel::moveEvent(QMoveEvent *)
{
	QtP6VXApplication::setSetting(QtP6VXApplication::keyKeyPanelPosition, pos());
}


void KeyPanel::showEvent(QShowEvent *)
{
	QtP6VXApplication::setSetting(QtP6VXApplication::keyKeyPanelVisible, true);
}

void KeyPanel::closeEvent(QCloseEvent *)
{
	QtP6VXApplication::setSetting(QtP6VXApplication::keyKeyPanelVisible, false);
}
