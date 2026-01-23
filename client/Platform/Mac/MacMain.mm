#import <AppKit/AppKit.h>
#include "../Platform.hpp"

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property(strong, nonatomic) NSWindow* window;
@end

@implementation AppDelegate
- (void)applicationDidFinishLaunching:(NSNotification*)notification
{
    NS_UNUSED(notification);
    clientPlatformInit();

    NSRect frame = NSMakeRect(0, 0, 1024, 720);
    NSWindowStyleMask style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
                              NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;
    self.window = [[NSWindow alloc] initWithContentRect:frame
                                              styleMask:style
                                                backing:NSBackingStoreBuffered
                                                  defer:NO];
    [self.window setTitle:@"Sony Headphones Client"];
    self.window.titleVisibility = NSWindowTitleHidden;
    self.window.titlebarAppearsTransparent = YES;
    self.window.toolbarStyle = NSWindowToolbarStyleUnified;
    [self.window center];
    [self.window setMinSize:NSMakeSize(900, 600)];

    NSSegmentedControl* tabs = [[NSSegmentedControl alloc]
        initWithLabels:@[@"Playback", @"Sound", @"Devices", @"System", @"About"]
          trackingMode:NSSegmentSwitchTrackingSelectOne
                target:nil
                action:nil];
    tabs.segmentStyle = NSSegmentStyleSeparated;
    tabs.selectedSegment = 0;
    tabs.translatesAutoresizingMaskIntoConstraints = NO;

    NSTitlebarAccessoryViewController* tabsAccessory = [[NSTitlebarAccessoryViewController alloc] init];
    tabsAccessory.view = tabs;
    tabsAccessory.layoutAttribute = NSLayoutAttributeCenterY;
    [self.window addTitlebarAccessoryViewController:tabsAccessory];

    NSView* contentView = [[NSView alloc] initWithFrame:frame];
    contentView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

    NSSplitView* splitView = [[NSSplitView alloc] initWithFrame:contentView.bounds];
    splitView.vertical = YES;
    splitView.dividerStyle = NSSplitViewDividerStyleThin;
    splitView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

    NSVisualEffectView* sidebar = [[NSVisualEffectView alloc] initWithFrame:NSMakeRect(0, 0, 260, frame.size.height)];
    sidebar.material = NSVisualEffectMaterialSidebar;
    sidebar.state = NSVisualEffectStateActive;
    sidebar.blendingMode = NSVisualEffectBlendingModeWithinWindow;
    sidebar.autoresizingMask = NSViewHeightSizable;

    NSVisualEffectView* mainBackground = [[NSVisualEffectView alloc] initWithFrame:NSMakeRect(0, 0, 740, frame.size.height)];
    mainBackground.material = NSVisualEffectMaterialWindowBackground;
    mainBackground.state = NSVisualEffectStateActive;
    mainBackground.blendingMode = NSVisualEffectBlendingModeWithinWindow;
    mainBackground.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

    [splitView addSubview:sidebar];
    [splitView addSubview:mainBackground];
    [splitView setPosition:260 ofDividerAtIndex:0];
    [contentView addSubview:splitView];

    NSStackView* sidebarStack = [[NSStackView alloc] initWithFrame:sidebar.bounds];
    sidebarStack.orientation = NSUserInterfaceLayoutOrientationVertical;
    sidebarStack.alignment = NSLayoutAttributeLeading;
    sidebarStack.spacing = 12.0;
    sidebarStack.edgeInsets = NSEdgeInsetsMake(20.0, 16.0, 20.0, 16.0);
    sidebarStack.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

    NSTextField* sidebarTitle = [NSTextField labelWithString:@"Devices"];
    sidebarTitle.font = [NSFont systemFontOfSize:13.0 weight:NSFontWeightSemibold];
    sidebarTitle.textColor = [NSColor secondaryLabelColor];

    NSBox* deviceBox = [NSBox box];
    deviceBox.title = @"Available";
    deviceBox.boxType = NSBoxCustom;
    deviceBox.borderColor = [NSColor separatorColor];
    deviceBox.borderWidth = 1.0;
    deviceBox.cornerRadius = 10.0;
    deviceBox.contentViewMargins = NSEdgeInsetsMake(12.0, 12.0, 12.0, 12.0);

    NSTextField* devicePlaceholder = [NSTextField labelWithString:@"No devices connected yet."];
    devicePlaceholder.textColor = [NSColor secondaryLabelColor];
    [deviceBox setContentView:devicePlaceholder];

    NSButton* connectButton = [NSButton buttonWithTitle:@"Connect" target:nil action:nil];
    connectButton.bezelStyle = NSBezelStyleRounded;
    connectButton.controlSize = NSControlSizeRegular;
    connectButton.keyEquivalent = @"\r";

    NSButton* refreshButton = [NSButton buttonWithTitle:@"Refresh" target:nil action:nil];
    refreshButton.bezelStyle = NSBezelStyleRounded;
    refreshButton.controlSize = NSControlSizeRegular;

    NSStackView* buttonRow = [NSStackView stackViewWithViews:@[connectButton, refreshButton]];
    buttonRow.orientation = NSUserInterfaceLayoutOrientationHorizontal;
    buttonRow.alignment = NSLayoutAttributeCenterY;
    buttonRow.distribution = NSStackViewDistributionFillEqually;
    buttonRow.spacing = 8.0;

    [sidebarStack addArrangedSubview:sidebarTitle];
    [sidebarStack addArrangedSubview:deviceBox];
    [sidebarStack addArrangedSubview:buttonRow];
    [sidebar addSubview:sidebarStack];

    NSStackView* contentStack = [[NSStackView alloc] initWithFrame:mainBackground.bounds];
    contentStack.orientation = NSUserInterfaceLayoutOrientationVertical;
    contentStack.alignment = NSLayoutAttributeLeading;
    contentStack.spacing = 16.0;
    contentStack.edgeInsets = NSEdgeInsetsMake(24.0, 24.0, 24.0, 24.0);
    contentStack.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

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

    NSBox* statusBox = [NSBox box];
    statusBox.title = @"Status";
    statusBox.boxType = NSBoxCustom;
    statusBox.borderColor = [NSColor separatorColor];
    statusBox.borderWidth = 1.0;
    statusBox.cornerRadius = 10.0;
    statusBox.contentViewMargins = NSEdgeInsetsMake(12.0, 12.0, 12.0, 12.0);

    NSTextField* statusPlaceholder = [NSTextField labelWithString:@"Waiting for a connected headset."];
    statusPlaceholder.textColor = [NSColor secondaryLabelColor];
    [statusBox setContentView:statusPlaceholder];

    NSBox* nowPlayingBox = [NSBox box];
    nowPlayingBox.title = @"Now Playing";
    nowPlayingBox.boxType = NSBoxCustom;
    nowPlayingBox.borderColor = [NSColor separatorColor];
    nowPlayingBox.borderWidth = 1.0;
    nowPlayingBox.cornerRadius = 10.0;
    nowPlayingBox.contentViewMargins = NSEdgeInsetsMake(12.0, 12.0, 12.0, 12.0);

    NSTextField* nowPlayingPlaceholder = [NSTextField labelWithString:@"Nothing is playing yet."];
    nowPlayingPlaceholder.textColor = [NSColor secondaryLabelColor];
    [nowPlayingBox setContentView:nowPlayingPlaceholder];

    [contentStack addArrangedSubview:titleStack];
    [contentStack addArrangedSubview:statusBox];
    [contentStack addArrangedSubview:nowPlayingBox];
    [mainBackground addSubview:contentStack];

    [self.window setContentView:contentView];
    [self.window makeKeyAndOrderFront:nil];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [NSApp activateIgnoringOtherApps:YES];
}

- (void)applicationWillTerminate:(NSNotification*)notification
{
    NS_UNUSED(notification);
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
