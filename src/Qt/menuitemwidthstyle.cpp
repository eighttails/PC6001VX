#include <QComboBox>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QStyleOption>
#include <QWidget>

#include "menuitemwidthstyle.h"

QSize MenuItemWidthStyle::sizeFromContents( ContentsType type, const QStyleOption* option,
											const QSize& contentsSize, const QWidget* widget ) const
{
	QSize size = QProxyStyle::sizeFromContents( type, option, contentsSize, widget );
	if( type != CT_MenuItem || qobject_cast<const QComboBox*>( widget ) ) return size;

	auto menuItem = qstyleoption_cast<const QStyleOptionMenuItem*>( option );
	if( !menuItem || menuItem->menuItemType == QStyleOptionMenuItem::Separator ) return size;

	// QFusionStyle::drawControl(CE_MenuItem)の配置計算に合わせる
	const qreal scale = QCoreApplication::testAttribute( Qt::AA_Use96Dpi )
						? 1.0 : menuItem->fontMetrics.fontDpi() / 96.0;
	const int h        = size.height();
	const int checkCol = qMax( int(h * 0.79), qMax( menuItem->maxIconWidth, int(21 * scale) ) );
	const int textLeft = checkCol + 7;	// checkColHOffset(4) + checkcol + menuItemHMargin(3)
	const int textW    = contentsSize.width() + 1;
	int required = textLeft + textW + 13;	// menuRightBorder(15) - 2
	if( menuItem->menuItemType == QStyleOptionMenuItem::SubMenu ){
		// サブメニュー矢印と重ならないようにする
		const int arrowW = 3 + (h - 4) / 2;
		required = qMax( required, textLeft + textW + arrowW + int(4 * scale) );
	}

	// 画面幅を超えないように制限する
	// (超える場合はQMenuのウィンドウだけが縮められ項目の右端が欠けるため、
	//  項目幅自体を画面内に収めて省略表示させる)
	int width = qMax( size.width(), required );
	const QScreen* screen = widget ? widget->screen() : QGuiApplication::primaryScreen();
	if( screen ){
		QStyleOption menuOpt;
		if( widget ) menuOpt.initFrom( widget );
		const int frame = 2 * ( pixelMetric( PM_MenuPanelWidth, &menuOpt, widget )
							  + pixelMetric( PM_MenuHMargin, &menuOpt, widget )
							  + pixelMetric( PM_MenuDesktopFrameWidth, &menuOpt, widget ) )
						+ sizeFromContents( CT_Menu, &menuOpt, QSize( 0, 0 ), widget ).width();
		width = qMax( qMin( width, screen->availableGeometry().width() - frame ), textLeft + h );
	}

	size.setWidth( width );
	return size;
}
