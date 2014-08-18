#include "emulationadaptor.h"
#include "../p6el.h"

EmulationAdaptor::EmulationAdaptor(QObject *parent)
	: QObject(parent)
	, P6Core(NULL)
	, RetCode(EL6::Quit)
{
}

void EmulationAdaptor::doEventLoop()
{
	RetCode = P6Core->EventLoop();
	emit finished();
}
