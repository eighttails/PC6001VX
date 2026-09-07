////////////////////////////////////////////////////////////////
// shareutils_ios.mm
//  ShareFile()のiOS向け実装(UIActivityViewController)
////////////////////////////////////////////////////////////////
#include "./shareutils.h"

#ifdef Q_OS_IOS

#import <UIKit/UIKit.h>

bool ShareFile( const QString& filePath, const QString& title, const QString& mimeType )
{
	Q_UNUSED(title)
	Q_UNUSED(mimeType)

	NSString* nsFilePath = filePath.toNSString();
	NSURL* nsFileUrl = [NSURL fileURLWithPath:nsFilePath];
	if( nsFileUrl == nil ){
		return false;
	}

	NSArray* sharingItems = @[nsFileUrl];
	UIActivityViewController* activityController =
			[[UIActivityViewController alloc] initWithActivityItems:sharingItems applicationActivities:nil];

	UIViewController* qtUIViewController = [[[UIApplication sharedApplication] windows] firstObject].rootViewController;
	if( qtUIViewController == nil ){
		return false;
	}

	if( [activityController respondsToSelector:@selector(popoverPresentationController)] ){
		activityController.popoverPresentationController.sourceView = qtUIViewController.view;
	}

	[qtUIViewController presentViewController:activityController animated:YES completion:nil];

	return true;
}

#endif
