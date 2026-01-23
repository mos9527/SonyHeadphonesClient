#import <AppKit/AppKit.h>
#include "../Platform.hpp"

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property(strong, nonatomic) NSWindow* window;
@end

@implementation AppDelegate
- (void)applicationDidFinishLaunching:(NSNotification*)notification
{
    (void)notification;
    clientPlatformInit();

    NSRect frame = NSMakeRect(0, 0, 1024, 720);
    NSWindowStyleMask style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                              NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;
    self.window = [[NSWindow alloc] initWithContentRect:frame
                                              styleMask:style
                                                backing:NSBackingStoreBuffered
                                                  defer:NO];
    [self.window setTitle:@"Sony Headphones Client"];
    [self.window center];
    [self.window setMinSize:NSMakeSize(900, 600)];

    NSVisualEffectView* background = [[NSVisualEffectView alloc] initWithFrame:frame];
    background.material = NSVisualEffectMaterialSidebar;
    background.state = NSVisualEffectStateActive;
    background.blendingMode = NSVisualEffectBlendingModeBehindWindow;
    background.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

    NSStackView* rootStack = [[NSStackView alloc] init];
    rootStack.orientation = NSUserInterfaceLayoutOrientationVertical;
    rootStack.alignment = NSLayoutAttributeLeading;
    rootStack.distribution = NSStackViewDistributionFill;
    rootStack.spacing = 16.0;
    rootStack.edgeInsets = NSEdgeInsetsMake(24.0, 24.0, 24.0, 24.0);
    rootStack.translatesAutoresizingMaskIntoConstraints = NO;

    NSTextField* title = [NSTextField labelWithString:@"Sony Headphones Client"];
    title.font = [NSFont systemFontOfSize:24.0 weight:NSFontWeightSemibold];
    title.textColor = [NSColor labelColor];

    NSTextField* subtitle = [NSTextField labelWithString:@"Connect, control, and fine-tune your headphones"];
    subtitle.font = [NSFont systemFontOfSize:13.0 weight:NSFontWeightRegular];
    subtitle.textColor = [NSColor secondaryLabelColor];

    NSStackView* titleStack = [NSStackView stackViewWithViews:@[title, subtitle]];
    titleStack.orientation = NSUserInterfaceLayoutOrientationVertical;
    titleStack.alignment = NSLayoutAttributeLeading;
    titleStack.spacing = 4.0;

    NSSegmentedControl* tabs = [[NSSegmentedControl alloc]
        initWithLabels:@[@"Playback", @"Sound", @"Devices", @"System", @"About"]
          trackingMode:NSSegmentSwitchTrackingSelectOne
                target:nil
                action:nil];
    tabs.segmentStyle = NSSegmentStyleSeparated;
    tabs.selectedSegment = 0;

    NSBox* deviceBox = [NSBox box];
    deviceBox.title = @"Devices";
    deviceBox.boxType = NSBoxCustom;
    deviceBox.borderColor = [NSColor colorWithWhite:0.82 alpha:1.0];
    deviceBox.borderWidth = 1.0;
    deviceBox.cornerRadius = 12.0;
    deviceBox.contentViewMargins = NSEdgeInsetsMake(16.0, 16.0, 16.0, 16.0);

    NSTextField* devicePlaceholder = [NSTextField labelWithString:@"No devices connected yet."];
    devicePlaceholder.textColor = [NSColor secondaryLabelColor];
    [deviceBox setContentView:devicePlaceholder];

    NSButton* connectButton = [NSButton buttonWithTitle:@"Connect" target:nil action:nil];
    connectButton.bezelStyle = NSBezelStyleRounded;
    connectButton.controlSize = NSControlSizeLarge;
    connectButton.keyEquivalent = @"\r";

    NSButton* refreshButton = [NSButton buttonWithTitle:@"Refresh" target:nil action:nil];
    refreshButton.bezelStyle = NSBezelStyleRounded;
    refreshButton.controlSize = NSControlSizeLarge;

    NSStackView* buttonRow = [NSStackView stackViewWithViews:@[connectButton, refreshButton]];
    buttonRow.orientation = NSUserInterfaceLayoutOrientationHorizontal;
    buttonRow.alignment = NSLayoutAttributeCenterY;
    buttonRow.distribution = NSStackViewDistributionFill;
    buttonRow.spacing = 10.0;

    NSBox* nowPlayingBox = [NSBox box];
    nowPlayingBox.title = @"Now Playing";
    nowPlayingBox.boxType = NSBoxCustom;
    nowPlayingBox.borderColor = [NSColor colorWithWhite:0.82 alpha:1.0];
    nowPlayingBox.borderWidth = 1.0;
    nowPlayingBox.cornerRadius = 12.0;
    nowPlayingBox.contentViewMargins = NSEdgeInsetsMake(16.0, 16.0, 16.0, 16.0);

    NSTextField* nowPlayingPlaceholder = [NSTextField labelWithString:@"Nothing is playing yet."];
    nowPlayingPlaceholder.textColor = [NSColor secondaryLabelColor];
    [nowPlayingBox setContentView:nowPlayingPlaceholder];

    [rootStack addArrangedSubview:titleStack];
    [rootStack addArrangedSubview:tabs];
    [rootStack addArrangedSubview:deviceBox];
    [rootStack addArrangedSubview:buttonRow];
    [rootStack addArrangedSubview:nowPlayingBox];

    [background addSubview:rootStack];
    [NSLayoutConstraint activateConstraints:@[
        [rootStack.leadingAnchor constraintEqualToAnchor:background.leadingAnchor],
        [rootStack.trailingAnchor constraintEqualToAnchor:background.trailingAnchor],
        [rootStack.topAnchor constraintEqualToAnchor:background.topAnchor],
        [rootStack.bottomAnchor constraintEqualToAnchor:background.bottomAnchor]
    ]];

    [self.window setContentView:background];
    [self.window makeKeyAndOrderFront:nil];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [NSApp activateIgnoringOtherApps:YES];
}

- (void)applicationWillTerminate:(NSNotification*)notification
{
    (void)notification;
    clientPlatformDestroy();
}
@end

int main(int argc, const char* argv[])
{
    @autoreleasepool
    {
        NSApplication* app = [NSApplication sharedApplication];
        AppDelegate* delegate = [[AppDelegate alloc] init];
        [app setDelegate:delegate];
        [app run];
    }
    return 0;
}
