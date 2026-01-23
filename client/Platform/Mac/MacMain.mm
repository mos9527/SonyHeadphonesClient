#import <AppKit/AppKit.h>
#include "../Platform.hpp"

static constexpr CGFloat kSidebarRowHeight = 28.0;
static constexpr CGFloat kSidebarCellWidth = 220.0;
static constexpr CGFloat kSidebarCellPadding = 8.0;
static constexpr CGFloat kSidebarFontSize = 13.0;
static constexpr CGFloat kSidebarTitleFontSize = 12.0;
static constexpr CGFloat kSidebarMinWidth = 220.0;
static constexpr CGFloat kSidebarMaxWidth = 320.0;
static constexpr CGFloat kSidebarStackSpacing = 12.0;
static constexpr CGFloat kSidebarStackMargin = 16.0;
static constexpr CGFloat kSidebarStackMarginTrailing = 14.0;
static constexpr CGFloat kBoxCornerRadius = 10.0;
static constexpr CGFloat kBoxContentMargin = 12.0;

@interface SidebarDataSource : NSObject <NSTableViewDataSource, NSTableViewDelegate>
@property(nonatomic, strong) NSArray<NSString*>* items;
@end

@implementation SidebarDataSource
- (instancetype)init
{
    self = [super init];
    if (self)
        _items = @[@"Overview", @"Playback", @"Sound", @"Devices", @"System", @"About"];
    return self;
}

- (NSInteger)numberOfRowsInTableView:(NSTableView*)tableView
{
    NS_UNUSED(tableView);
    return static_cast<NSInteger>(self.items.count);
}

- (NSView*)tableView:(NSTableView*)tableView viewForTableColumn:(NSTableColumn*)tableColumn row:(NSInteger)row
{
    NS_UNUSED(tableView);
    NS_UNUSED(tableColumn);
    if (row == NSNotFound || static_cast<NSUInteger>(row) >= self.items.count)
        return nil;
    NSTableCellView* cell = [tableView makeViewWithIdentifier:@"SidebarCell" owner:self];
    if (!cell)
    {
        cell = [[NSTableCellView alloc] initWithFrame:NSMakeRect(0, 0, kSidebarCellWidth, kSidebarRowHeight)];
        cell.identifier = @"SidebarCell";
        NSTextField* textField = [NSTextField labelWithString:@""];
        textField.translatesAutoresizingMaskIntoConstraints = NO;
        textField.font = [NSFont systemFontOfSize:kSidebarFontSize weight:NSFontWeightRegular];
        textField.textColor = [NSColor labelColor];
        cell.textField = textField;
        [cell addSubview:textField];
        [NSLayoutConstraint activateConstraints:@[
            [textField.leadingAnchor constraintEqualToAnchor:cell.leadingAnchor constant:kSidebarCellPadding],
            [textField.centerYAnchor constraintEqualToAnchor:cell.centerYAnchor]
        ]];
    }
    cell.textField.stringValue = self.items[static_cast<NSUInteger>(row)];
    return cell;
}

- (CGFloat)tableView:(NSTableView*)tableView heightOfRow:(NSInteger)row
{
    NS_UNUSED(tableView);
    NS_UNUSED(row);
    return kSidebarRowHeight;
}
@end

@interface AppDelegate : NSObject <NSApplicationDelegate>
@property(strong, nonatomic) NSWindow* window;
@property(strong, nonatomic) SidebarDataSource* sidebarDataSource;
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

    NSSearchField* searchField = [[NSSearchField alloc] initWithFrame:NSMakeRect(0, 0, 220, 24)];
    searchField.placeholderString = @"Search";
    searchField.controlSize = NSControlSizeSmall;
    searchField.font = [NSFont systemFontOfSize:[NSFont smallSystemFontSize]];
    searchField.translatesAutoresizingMaskIntoConstraints = NO;
    NSTitlebarAccessoryViewController* searchAccessory = [[NSTitlebarAccessoryViewController alloc] init];
    searchAccessory.view = searchField;
    searchAccessory.layoutAttribute = NSLayoutAttributeRight;
    [self.window addTitlebarAccessoryViewController:searchAccessory];

    NSSplitViewController* splitController = [[NSSplitViewController alloc] init];
    splitController.splitView.vertical = YES;
    splitController.splitView.dividerStyle = NSSplitViewDividerStyleThin;

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

    NSViewController* sidebarController = [[NSViewController alloc] init];
    sidebarController.view = sidebar;
    NSViewController* contentController = [[NSViewController alloc] init];
    contentController.view = mainBackground;
    NSSplitViewItem* sidebarItem = [NSSplitViewItem sidebarWithViewController:sidebarController];
    NSSplitViewItem* contentItem = [NSSplitViewItem viewController:contentController];
    sidebarItem.minimumThickness = kSidebarMinWidth;
    sidebarItem.maximumThickness = kSidebarMaxWidth;
    [splitController addSplitViewItem:sidebarItem];
    [splitController addSplitViewItem:contentItem];

    NSStackView* sidebarStack = [[NSStackView alloc] initWithFrame:sidebar.bounds];
    sidebarStack.orientation = NSUserInterfaceLayoutOrientationVertical;
    sidebarStack.alignment = NSLayoutAttributeLeading;
    sidebarStack.spacing = kSidebarStackSpacing;
    sidebarStack.edgeInsets = NSEdgeInsetsMake(kSidebarStackMargin, kSidebarStackMarginTrailing,
                                               kSidebarStackMargin, kSidebarStackMarginTrailing);
    sidebarStack.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

    NSTextField* sidebarTitle = [NSTextField labelWithString:@"Navigation"];
    sidebarTitle.font = [NSFont systemFontOfSize:kSidebarTitleFontSize weight:NSFontWeightSemibold];
    sidebarTitle.textColor = [NSColor secondaryLabelColor];

    NSTableView* sidebarTable = [[NSTableView alloc] initWithFrame:NSMakeRect(0, 0, 240, 200)];
    sidebarTable.headerView = nil;
    sidebarTable.usesAlternatingRowBackgroundColors = NO;
    sidebarTable.selectionHighlightStyle = NSTableViewSelectionHighlightStyleSourceList;
    sidebarTable.rowSizeStyle = NSTableViewRowSizeStyleDefault;
    if (@available(macOS 11.0, *))
        sidebarTable.style = NSTableViewStyleSidebar;

    NSTableColumn* sidebarColumn = [[NSTableColumn alloc] initWithIdentifier:@"Title"];
    sidebarColumn.width = kSidebarMinWidth;
    [sidebarTable addTableColumn:sidebarColumn];
    self.sidebarDataSource = [[SidebarDataSource alloc] init];
    sidebarTable.dataSource = self.sidebarDataSource;
    sidebarTable.delegate = self.sidebarDataSource;
    [sidebarTable reloadData];
    [sidebarTable selectRowIndexes:[NSIndexSet indexSetWithIndex:0] byExtendingSelection:NO];

    NSScrollView* sidebarScroll = [[NSScrollView alloc] initWithFrame:NSMakeRect(0, 0, 240, 200)];
    sidebarScroll.hasVerticalScroller = YES;
    sidebarScroll.borderType = NSNoBorder;
    sidebarScroll.drawsBackground = NO;
    sidebarScroll.documentView = sidebarTable;

    NSBox* deviceBox = [NSBox box];
    deviceBox.title = @"Available";
    deviceBox.boxType = NSBoxCustom;
    deviceBox.borderColor = [NSColor separatorColor];
    deviceBox.borderWidth = 1.0;
    deviceBox.cornerRadius = kBoxCornerRadius;
    deviceBox.contentViewMargins = NSEdgeInsetsMake(kBoxContentMargin, kBoxContentMargin,
                                                    kBoxContentMargin, kBoxContentMargin);

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
    [sidebarStack addArrangedSubview:sidebarScroll];
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
    statusBox.cornerRadius = kBoxCornerRadius;
    statusBox.contentViewMargins = NSEdgeInsetsMake(kBoxContentMargin, kBoxContentMargin,
                                                    kBoxContentMargin, kBoxContentMargin);

    NSTextField* statusPlaceholder = [NSTextField labelWithString:@"Waiting for a connected headset."];
    statusPlaceholder.textColor = [NSColor secondaryLabelColor];
    [statusBox setContentView:statusPlaceholder];

    NSBox* quickActionsBox = [NSBox box];
    quickActionsBox.title = @"Quick Actions";
    quickActionsBox.boxType = NSBoxCustom;
    quickActionsBox.borderColor = [NSColor separatorColor];
    quickActionsBox.borderWidth = 1.0;
    quickActionsBox.cornerRadius = kBoxCornerRadius;
    quickActionsBox.contentViewMargins = NSEdgeInsetsMake(kBoxContentMargin, kBoxContentMargin,
                                                          kBoxContentMargin, kBoxContentMargin);

    NSButton* playPauseButton = [NSButton buttonWithTitle:@"Play / Pause" target:nil action:nil];
    playPauseButton.bezelStyle = NSBezelStyleTexturedRounded;
    NSButton* locateButton = [NSButton buttonWithTitle:@"Find Headphones" target:nil action:nil];
    locateButton.bezelStyle = NSBezelStyleTexturedRounded;
    NSStackView* quickActionsStack = [NSStackView stackViewWithViews:@[playPauseButton, locateButton]];
    quickActionsStack.orientation = NSUserInterfaceLayoutOrientationVertical;
    quickActionsStack.alignment = NSLayoutAttributeLeading;
    quickActionsStack.spacing = 8.0;
    [quickActionsBox setContentView:quickActionsStack];

    NSBox* nowPlayingBox = [NSBox box];
    nowPlayingBox.title = @"Now Playing";
    nowPlayingBox.boxType = NSBoxCustom;
    nowPlayingBox.borderColor = [NSColor separatorColor];
    nowPlayingBox.borderWidth = 1.0;
    nowPlayingBox.cornerRadius = kBoxCornerRadius;
    nowPlayingBox.contentViewMargins = NSEdgeInsetsMake(kBoxContentMargin, kBoxContentMargin,
                                                        kBoxContentMargin, kBoxContentMargin);

    NSTextField* nowPlayingPlaceholder = [NSTextField labelWithString:@"Nothing is playing yet."];
    nowPlayingPlaceholder.textColor = [NSColor secondaryLabelColor];
    [nowPlayingBox setContentView:nowPlayingPlaceholder];

    [contentStack addArrangedSubview:titleStack];
    [contentStack addArrangedSubview:statusBox];
    [contentStack addArrangedSubview:quickActionsBox];
    [contentStack addArrangedSubview:nowPlayingBox];
    [mainBackground addSubview:contentStack];

    self.window.contentViewController = splitController;
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
