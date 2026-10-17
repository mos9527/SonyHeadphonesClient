#include "../Platform.hpp"
#include <SDL3/SDL.h>
#include <mdr/Protocol.hpp>
#import <CoreFoundation/CoreFoundation.h>
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <dispatch/dispatch.h>
#include <algorithm>
#include <cstring>
#include <mutex>

@interface SHCShareRequest : NSObject
@property (nonatomic, copy) NSString* filename;
@property (nonatomic, strong) NSData* data;
@property (nonatomic, weak) UIWindow* window;
@property (nonatomic, weak) UIActivityViewController* controller;
@property (nonatomic, assign) ClientPlatformSaveBlobCallback callback;
@property (nonatomic, assign) void* userdata;
@end

@implementation SHCShareRequest
@end

namespace
{
    constexpr Uint64 kEventWaitRecheckNS = SDL_MS_TO_NS(50);
    CFRunLoopSourceRef gEventWakeupSource{};

    void PerformEventWakeup(void*)
    {
    }

    bool SDLCALL WakeEventWait(void* userdata, SDL_Event*)
    {
        CFRunLoopSourceSignal(static_cast<CFRunLoopSourceRef>(userdata));
        CFRunLoopWakeUp(CFRunLoopGetMain());
        return true;
    }

    constexpr NSTimeInterval kRetentionSeconds = 7 * 24 * 60 * 60;
    std::mutex gShareMutex;
    SHCShareRequest* gShare;
    bool gShareClosed{};

    dispatch_queue_t FileQueue()
    {
        static dispatch_queue_t queue;
        static dispatch_once_t once;
        dispatch_once(&once, ^{
            queue = dispatch_queue_create("com.mos9527.SonyHeadphonesClient.exports", DISPATCH_QUEUE_SERIAL);
        });
        return queue;
    }

    bool IsCurrent(SHCShareRequest* request)
    {
        std::scoped_lock lock(gShareMutex);
        return gShare == request;
    }

    void FinishShare(SHCShareRequest* request, ClientPlatformSaveBlobResult result, NSString* error = nil)
    {
        {
            std::scoped_lock lock(gShareMutex);
            if (gShare != request)
                return;
            gShare = nil;
        }
        request.callback(request.userdata, result, request.filename.UTF8String, error.UTF8String);
        if (SDL_WasInit(SDL_INIT_VIDEO))
        {
            SDL_Event wake{};
            wake.type = SDL_EVENT_USER;
            SDL_PushEvent(&wake);
        }
    }

    NSString* ErrorText(NSError* error)
    {
        return error.localizedDescription ?: @"Unable to export file";
    }

    void RemoveDirectory(NSURL* directory)
    {
        NSError* error = nil;
        if (directory && ![[NSFileManager defaultManager] removeItemAtURL:directory error:&error])
            MDR_LOG("Unable to remove iOS export cache: {}", ErrorText(error).UTF8String);
    }

    void DiscardDirectory(NSURL* directory)
    {
        dispatch_async(FileQueue(), ^{
            @autoreleasepool
            {
                RemoveDirectory(directory);
            }
        });
    }

    void CleanCache(NSFileManager* manager, NSURL* root)
    {
        static bool cleaned{};
        if (cleaned)
            return;
        cleaned = true;
        NSArray<NSURL*>* directories = [manager contentsOfDirectoryAtURL:root
            includingPropertiesForKeys:@[NSURLIsDirectoryKey, NSURLIsSymbolicLinkKey, NSURLContentModificationDateKey]
            options:NSDirectoryEnumerationSkipsHiddenFiles error:nullptr];
        NSDate* cutoff = [NSDate dateWithTimeIntervalSinceNow:-kRetentionSeconds];
        for (NSURL* directory in directories)
        {
            if (![directory.lastPathComponent hasPrefix:@"export-"])
                continue;
            NSNumber* isDirectory = nil;
            NSNumber* isSymbolicLink = nil;
            NSDate* modified = nil;
            if ([directory getResourceValue:&isDirectory forKey:NSURLIsDirectoryKey error:nullptr] &&
                [directory getResourceValue:&isSymbolicLink forKey:NSURLIsSymbolicLinkKey error:nullptr] &&
                [directory getResourceValue:&modified forKey:NSURLContentModificationDateKey error:nullptr] &&
                isDirectory.boolValue && !isSymbolicLink.boolValue && modified &&
                [modified compare:cutoff] == NSOrderedAscending)
                RemoveDirectory(directory);
        }
    }

    void PresentShare(SHCShareRequest* request, NSURL* file, NSURL* directory)
    {
        if (!IsCurrent(request))
        {
            DiscardDirectory(directory);
            return;
        }
        UIWindow* window = request.window;
        UIViewController* presenter = window.rootViewController;
        while (presenter.presentedViewController && !presenter.presentedViewController.isBeingDismissed)
            presenter = presenter.presentedViewController;
        if (!window || window.hidden ||
            (window.windowScene && window.windowScene.activationState != UISceneActivationStateForegroundActive) ||
            [UIApplication sharedApplication].applicationState != UIApplicationStateActive || !presenter ||
            presenter.isBeingPresented || presenter.isBeingDismissed || presenter.presentedViewController ||
            !presenter.view.window || [presenter isKindOfClass:[UIActivityViewController class]] ||
            [presenter isKindOfClass:[UIAlertController class]])
        {
            FinishShare(request, CLIENT_PLATFORM_SAVE_BLOB_FAILED, @"No active view is available for file sharing");
            DiscardDirectory(directory);
            return;
        }
        UIActivityViewController* controller = [[UIActivityViewController alloc]
            initWithActivityItems:@[file] applicationActivities:nil];
        if (!controller)
        {
            FinishShare(request, CLIENT_PLATFORM_SAVE_BLOB_FAILED, @"Unable to create the share panel");
            DiscardDirectory(directory);
            return;
        }
        request.controller = controller;
        controller.completionWithItemsHandler = ^(UIActivityType activityType, BOOL completed,
                                                  NSArray* returnedItems, NSError* error) {
            if (error)
                FinishShare(request, CLIENT_PLATFORM_SAVE_BLOB_FAILED, ErrorText(error));
            else
                FinishShare(request, completed ? CLIENT_PLATFORM_SAVE_BLOB_DISPATCHED : CLIENT_PLATFORM_SAVE_BLOB_CANCELLED);
        };
        controller.modalPresentationStyle = UIModalPresentationPopover;
        UIPopoverPresentationController* popover = controller.popoverPresentationController;
        popover.sourceView = presenter.view;
        const CGRect bounds = presenter.view.bounds;
        popover.sourceRect = CGRectMake(CGRectGetMidX(bounds), CGRectGetMidY(bounds), 1, 1);
        popover.permittedArrowDirections = 0;
        [presenter presentViewController:controller animated:YES completion:^{
            if (IsCurrent(request) && !controller.presentingViewController)
            {
                controller.completionWithItemsHandler = nil;
                FinishShare(request, CLIENT_PLATFORM_SAVE_BLOB_FAILED, @"Unable to present the share panel");
                DiscardDirectory(directory);
            }
        }];
    }

    void PrepareShare(SHCShareRequest* request)
    {
        if (!IsCurrent(request))
            return;
        NSFileManager* manager = [NSFileManager defaultManager];
        NSURL* cache = [[manager URLsForDirectory:NSCachesDirectory inDomains:NSUserDomainMask] firstObject];
        NSURL* root = [cache URLByAppendingPathComponent:@"exports" isDirectory:YES];
        NSError* error = nil;
        if (!root || ![manager createDirectoryAtURL:root withIntermediateDirectories:YES attributes:nil error:&error])
        {
            request.data = nil;
            FinishShare(request, CLIENT_PLATFORM_SAVE_BLOB_FAILED, ErrorText(error));
            return;
        }
        CleanCache(manager, root);
        NSString* name = [@"export-" stringByAppendingString:[NSUUID UUID].UUIDString];
        NSURL* directory = [root URLByAppendingPathComponent:name isDirectory:YES];
        if (![manager createDirectoryAtURL:directory withIntermediateDirectories:NO attributes:nil error:&error])
        {
            request.data = nil;
            FinishShare(request, CLIENT_PLATFORM_SAVE_BLOB_FAILED, ErrorText(error));
            return;
        }
        NSURL* file = [directory URLByAppendingPathComponent:request.filename isDirectory:NO];
        const bool written = [request.data writeToURL:file options:NSDataWritingAtomic error:&error];
        request.data = nil;
        if (!written || !IsCurrent(request))
        {
            if (!written)
                FinishShare(request, CLIENT_PLATFORM_SAVE_BLOB_FAILED, ErrorText(error));
            RemoveDirectory(directory);
            return;
        }
        dispatch_async(dispatch_get_main_queue(), ^{
            @autoreleasepool
            {
                PresentShare(request, file, directory);
            }
        });
    }
}

bool clientPlatformInit()
{
    if (gEventWakeupSource)
        return true;
    if (!SDL_Init(SDL_INIT_EVENTS))
        return false;
    CFRunLoopSourceContext context{};
    context.perform = PerformEventWakeup;
    gEventWakeupSource = CFRunLoopSourceCreate(kCFAllocatorDefault, 0, &context);
    if (!gEventWakeupSource)
        return SDL_SetError("Unable to create the iOS event wakeup source");
    CFRunLoopAddSource(CFRunLoopGetMain(), gEventWakeupSource, kCFRunLoopCommonModes);
    if (!SDL_AddEventWatch(WakeEventWait, gEventWakeupSource))
    {
        CFRunLoopSourceInvalidate(gEventWakeupSource);
        CFRelease(gEventWakeupSource);
        gEventWakeupSource = nullptr;
        return false;
    }
    return true;
}

bool clientPlatformWaitEventTimeout(int timeoutMS)
{
    const Uint64 deadline = timeoutMS > 0 ? SDL_GetTicksNS() + SDL_MS_TO_NS(timeoutMS) : 0;
    while (true)
    {
        @autoreleasepool
        {
            SDL_PumpEvents();
            const int queued = SDL_PeepEvents(nullptr, 0, SDL_PEEKEVENT, SDL_EVENT_FIRST, SDL_EVENT_LAST);
            if (queued != 0)
                return queued > 0;
            const Uint64 now = SDL_GetTicksNS();
            if (timeoutMS == 0 || (timeoutMS > 0 && now >= deadline))
                return false;
            const Uint64 waitNS = timeoutMS < 0 ? kEventWaitRecheckNS : std::min(kEventWaitRecheckNS, deadline - now);
            const SInt32 result = CFRunLoopRunInMode(kCFRunLoopDefaultMode,
                static_cast<double>(waitNS) / SDL_NS_PER_SECOND, true);
            if (result == kCFRunLoopRunFinished || result == kCFRunLoopRunStopped)
                return SDL_SetError("The iOS event run loop stopped");
        }
    }
}

int clientPlatformSaveBlob(
    const char* filename, const unsigned char* data, size_t dataSize, const char* mimeType, SDL_Window* window,
    ClientPlatformSaveBlobCallback callback, void* userdata)
{
    if (!filename || !*filename || std::strlen(filename) > 255 || std::strpbrk(filename, "/\\") ||
        std::strcmp(filename, ".") == 0 || std::strcmp(filename, "..") == 0 || !data || !dataSize ||
        !mimeType || !*mimeType || !window || !callback)
    {
        SDL_SetError("Invalid file sharing arguments");
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    }
    if (!SDL_IsMainThread())
    {
        SDL_SetError("iOS file sharing must be requested on the SDL main thread");
        return MDR_RESULT_ERROR_INVALID_ARGUMENT;
    }
    @autoreleasepool
    {
        UIWindow* nativeWindow = (__bridge UIWindow*)SDL_GetPointerProperty(
            SDL_GetWindowProperties(window), SDL_PROP_WINDOW_UIKIT_WINDOW_POINTER, nullptr);
        NSString* name = [NSString stringWithUTF8String:filename];
        if (!nativeWindow || !name)
        {
            SDL_SetError("Unable to locate the iOS window or decode the export filename");
            return MDR_RESULT_ERROR_INVALID_ARGUMENT;
        }
        SHCShareRequest* request = [[SHCShareRequest alloc] init];
        request.filename = name;
        request.data = [NSData dataWithBytes:data length:dataSize];
        request.window = nativeWindow;
        request.callback = callback;
        request.userdata = userdata;
        if (!request || !request.data)
        {
            SDL_SetError("Unable to prepare the iOS export data");
            return MDR_RESULT_ERROR_GENERAL;
        }
        {
            std::scoped_lock lock(gShareMutex);
            if (gShareClosed || gShare)
            {
                SDL_SetError("iOS file sharing is unavailable or already active");
                return MDR_RESULT_ERROR_GENERAL;
            }
            gShare = request;
        }
        dispatch_async(FileQueue(), ^{
            @autoreleasepool
            {
                PrepareShare(request);
            }
        });
    }
    return MDR_RESULT_OK;
}

extern "C" void clientIOSDestroy()
{
    if (gEventWakeupSource)
    {
        SDL_RemoveEventWatch(WakeEventWait, gEventWakeupSource);
        CFRunLoopSourceInvalidate(gEventWakeupSource);
        CFRelease(gEventWakeupSource);
        gEventWakeupSource = nullptr;
    }
    @autoreleasepool
    {
        SHCShareRequest* request;
        {
            std::scoped_lock lock(gShareMutex);
            gShareClosed = true;
            request = gShare;
        }
        if (!request)
            return;
        FinishShare(request, CLIENT_PLATFORM_SAVE_BLOB_CANCELLED);
        dispatch_async(dispatch_get_main_queue(), ^{
            UIActivityViewController* controller = request.controller;
            controller.completionWithItemsHandler = nil;
            if (controller.presentingViewController)
                [controller dismissViewControllerAnimated:NO completion:nil];
        });
    }
}
