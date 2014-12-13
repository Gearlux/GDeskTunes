#include <AppKit/NSWindow.h>

#include "mediakeys.h"
#include "cocoainit.h"
#import <Cocoa/Cocoa.h>
#import "../thirdparty/SPMediaKeyTap/SPMediaKeyTap.h"

@interface SPMediaKeyTapExampleAppDelegate : NSObject <NSApplicationDelegate> {
@public
    MediaKeys::MediaKeys * mk;
}

@end

@implementation SPMediaKeyTapExampleAppDelegate
-(void)mediaKeyTap:(SPMediaKeyTap*)keyTap receivedMediaKeyEvent:(NSEvent*)event
{
    NSAssert([event type] == NSSystemDefined && [event subtype] == SPSystemDefinedEventMediaKeys, @"Unexpected NSEvent in mediaKeyTap:receivedMediaKeyEvent:");
    // here be dragons...
    int keyCode = (([event data1] & 0xFFFF0000) >> 16);
    int keyFlags = ([event data1] & 0x0000FFFF);
    BOOL keyIsPressed = (((keyFlags & 0xFF00) >> 8)) == 0xA;
    int keyRepeat = (keyFlags & 0x1);

    mk->receiveKey(keyCode, keyRepeat, keyIsPressed);
//        NSString *debugString = [NSString stringWithFormat:@"%@", keyRepeat?@", repeated.":@"."];
//        switch (keyCode) {
//            case NX_KEYTYPE_PLAY:
//                debugString = [@"Play/pause pressed" stringByAppendingString:debugString];
//                break;

//            case NX_KEYTYPE_FAST:
//                debugString = [@"Ffwd pressed" stringByAppendingString:debugString];
//                break;

//            case NX_KEYTYPE_REWIND:
//                debugString = [@"Rewind pressed" stringByAppendingString:debugString];
//                break;
//            default:
//                debugString = [NSString stringWithFormat:@"Key %d pressed%@", keyCode, debugString];
//                break;
//            // More cases defined in hidsystem/ev_keymap.h
//        }
}
//#define NX_KEYTYPE_PLAY			16
//#define NX_KEYTYPE_NEXT			17
//#define NX_KEYTYPE_PREVIOUS		18
//#define NX_KEYTYPE_FAST			19
//#define NX_KEYTYPE_REWIND		20
@end

class MediaKeys::Private{
public:
    SPMediaKeyTapExampleAppDelegate * delegate;
    SPMediaKeyTap * keyTap;
};

MediaKeys::MediaKeys(QWidget *, QObject *parent) :
QObject(parent)
{
    d = new MediaKeys::Private();
    CocoaInitializer initializer;
    d->delegate = [SPMediaKeyTapExampleAppDelegate alloc];
    d->delegate->mk = this;
    [[NSUserDefaults standardUserDefaults] registerDefaults:[NSDictionary dictionaryWithObjectsAndKeys:
        [SPMediaKeyTap defaultMediaKeyUserBundleIdentifiers], kMediaKeyUsingBundleIdentifiersDefaultsKey,
    nil]];

    d->keyTap = [[SPMediaKeyTap alloc] initWithDelegate: d->delegate];
    if([SPMediaKeyTap usesGlobalMediaKeyTap]){
        [d->keyTap startWatchingMediaKeys];
    }
    // Deactivate on background
    // win->installEventFilter(this);
}

bool MediaKeys::eventFilter(QObject */*object*/, QEvent *event)
{
    switch(event->type()){
    case QEvent::WindowActivate:
        [d->keyTap startWatchingMediaKeys];
        break;
    case QEvent::WindowDeactivate:
        [d->keyTap stopWatchingMediaKeys];
        break;
    default:
        break;
    }
    return false;
}

void MediaKeys::receiveKey(int keyCode, int keyRepeat, bool isUp){
    // XXX store lastdown repeat to reforward it as well in up and pressed?
    // Match as closely as possible the merguez behavior, even if it's brain dead for now
    emit this->keyReceived(keyCode, (keyRepeat == 1), !isUp);
}

