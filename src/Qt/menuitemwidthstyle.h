#ifndef MENUITEMWIDTHSTYLE_H
#define MENUITEMWIDTHSTYLE_H

#include <QProxyStyle>

///////////////////////////////////////////////////////////
// メニュー項目幅補正スタイル
//
// Androidでは基準DPIが72のため、Fusion系スタイルのメニュー項目幅計算
// (sizeFromContents)が描画時(drawControl)の必要幅より小さくなり、
// テキストが省略表示されてしまう。
// 描画時の配置計算に合わせて、画面に収まる範囲で項目幅を広げる。
///////////////////////////////////////////////////////////
class MenuItemWidthStyle : public QProxyStyle
{
public:
	using QProxyStyle::QProxyStyle;

	QSize sizeFromContents( ContentsType type, const QStyleOption* option,
							const QSize& contentsSize, const QWidget* widget ) const override;
};

#endif // MENUITEMWIDTHSTYLE_H
