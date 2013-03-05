#include "qtp6vxapplication.h"
#include <QKeyEvent>
#include "../event.h"
#include "../osd.h"

QtP6VXApplication::QtP6VXApplication(int &argc, char **argv) :
    QtSingleApplication(argc, argv)
{
}

bool QtP6VXApplication::notify ( QObject * receiver, QEvent * event )
{
    Event ev;
    ev.type = EV_NOEVENT;

    switch(event->type()){
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    {
        QKeyEvent* ke = static_cast<QKeyEvent*>(event);
        ev.type        = EV_KEYDOWN;
        ev.key.state   = event->type() == QEvent::KeyPress ? true : false;
        ev.key.sym     = OSD_ConvertKeyCode( ke->key() );
        ev.key.mod	   = (PCKEYmod)(
                    ( ke->modifiers() & Qt::ShiftModifier ? KVM_SHIFT : KVM_NONE )
                    | ( ke->modifiers() & Qt::ControlModifier ? KVM_CTRL : KVM_NONE )
                    | ( ke->modifiers() & Qt::AltModifier ? KVM_SHIFT : KVM_NONE )
                    | ( ke->modifiers() & Qt::MetaModifier ? KVM_META : KVM_NONE )
                    | ( ke->modifiers() & Qt::KeypadModifier ? KVM_NUM : KVM_NONE )
                    //#PENDING CAPSLOCKは検出できない？
                    //| ( ke->modifiers() & Qt::caps ? KVM_NUM : KVM_NONE )
                    );
        ev.key.unicode = ke->text().utf16()[0];
        OSD_PushEvent(ev.type);
        break;
    }
    case QEvent::ContextMenu:
    {
        ev.type        = EV_CONTEXTMENU;
        OSD_PushEvent(ev.type);
        break;
    }
    default:;
    }

    if(ev.type == EV_NOEVENT){
        return QtSingleApplication::notify(receiver, event);
    } else {
        return true;
    }
}
