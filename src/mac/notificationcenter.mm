#include "notificationcenter.h"

#include <QDebug>

#include <Foundation/Foundation.h>
#include <Foundation/NSUserNotification.h>

@interface NotificationCenter : NSObject<NSUserNotificationCenterDelegate>

+ (NotificationCenter *) center;
- (void) scheduleNotificationWithTitle:(NSString *)title artist:(NSString *)artist album:(NSString *)album imageURL:(NSString *)imageURL;

@end

@implementation NotificationCenter

+ (NotificationCenter *)center
{
    static dispatch_once_t token;
    static NotificationCenter *shared = nil;

    dispatch_once(&token, ^{
        shared = [[NotificationCenter alloc] init];
    });

    return shared;
}

- (id)init
{
    self = [super init];

    if (self)
    {
        [[NSUserNotificationCenter defaultUserNotificationCenter] setDelegate:self];
    }

    return self;
}

#pragma mark - Delegates

/*
 * NSUserNotificationCenter delegate
 */
- (void)userNotificationCenter:(NSUserNotificationCenter *)center didActivateNotification:(NSUserNotification *)notification
{
    qDebug() << "Delegate called";

    int type;

    switch (notification.activationType)
    {
        case NSUserNotificationActivationTypeActionButtonClicked:
            type = 1;
            break;

        case NSUserNotificationActivationTypeContentsClicked:
            type = 2;
            break;

        default:
            type = 0;
            break;
    }

    // [delegate notificationWasActivated:type];
}
@end

class MacNotificationCenter::Private
{
public:
    NotificationCenter *center;
};

MacNotificationCenter::MacNotificationCenter()
{
    d = new Private();
    d->center = [NotificationCenter center];
}

void MacNotificationCenter::showMessage(QString title, QString message, QString art, bool itunes, bool show_art)
{
    NSUserNotification *userNotification = [[[NSUserNotification alloc] init] autorelease];

     userNotification.title = [NSString stringWithCString:title.toUtf8().constData()];

     if (itunes) {
         userNotification.subtitle = [NSString stringWithCString:message.toUtf8().constData()];
         // [ userNotification setValue:@YES forKey:@"_showsButtons" ];
     }
     else {
         userNotification.informativeText = [NSString stringWithCString:message.toUtf8().constData()];
     }

     // Make sure the version of OS X supports this.
     if ([userNotification respondsToSelector:@selector(setContentImage:)])
     {
         // Try to load the album art if possible.
         if (show_art && art != QString::null)
         {
             NSImage *image = [[NSImage alloc] initWithContentsOfURL:[NSURL URLWithString:[NSString stringWithCString:art.toUtf8().constData()] ] ];

             if (itunes) {
                 [userNotification setValue:image forKey:@"_identityImage"];
             }
             else {
                 userNotification.contentImage = image;
             }
         }
     }

     userNotification.actionButtonTitle = @"Skip";

     // Remove the previous notifications in order to make this notification appear immediately.
     [[NSUserNotificationCenter defaultUserNotificationCenter] removeAllDeliveredNotifications];

     // Deliver the notification.
     [[NSUserNotificationCenter defaultUserNotificationCenter] scheduleNotification:userNotification];
}
