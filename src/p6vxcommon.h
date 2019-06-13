#ifndef P6VXCOMMON_H
#define P6VXCOMMON_H

#ifdef QTP6VX
#include <QCoreApplication>
#include <QString>
#include <QDebug>
//翻訳された文字列を返す
#define TRANS(a) (QCoreApplication::translate("PC6001VX", a).toUtf8().data())
#else
#define TRANS(a) a
#endif

#if (defined PANDORA) || (defined ANDROID)
#include <string>
#include <sstream>
namespace std{
template <typename T>
std::string to_string(T value)
{
	std::ostringstream os ;
	os << value ;
	return os.str() ;
}
}
#endif

enum TiltDirection{
	NEWTRAL,
	LEFT,
	RIGHT,
};
#ifdef QTP6VX
Q_DECLARE_METATYPE(TiltDirection)
#endif

// TILTモード用に画面を傾ける
void TiltScreen(TiltDirection dir);
void UpdateTilt();

#endif // P6VXCOMMON_H
