#include "platform/macos/MacOSBluetoothConnector.h"
#include "App.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_metal.h"

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>
#import <objc/runtime.h>

static void glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
// Hide Dock icon: https://github.com/glfw/glfw/issues/1552#issuecomment-766453125
@interface NSApplication (AppHideDockIcon)
- (BOOL) setActivationPolicyOverride: (NSApplicationActivationPolicy) activationPolicy;
@end
@implementation NSApplication (AppHideDockIcon)
+ (void) load {
    Method original = class_getInstanceMethod(self, @selector(setActivationPolicy:));
    Method swizzled = class_getInstanceMethod(self, @selector(setActivationPolicyOverride:));
    method_exchangeImplementations(original, swizzled);
}
- (BOOL) setActivationPolicyOverride: (NSApplicationActivationPolicy) activationPolicy {
    return [self setActivationPolicyOverride: NSApplicationActivationPolicyAccessory];
}
@end

// Menu bar callback
static bool clickHandled = true;
@interface StatusBarButtonCallback : NSObject
+ (void) buttonAction:(id)sender;
@end
@implementation StatusBarButtonCallback
+ (void) buttonAction:(id)sender {
    clickHandled = false;
}
@end
void EnterGUIMainLoop(BluetoothWrapper bt){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return;
    
    // Create window with graphics context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(GUI_WIDTH, GUI_HEIGHT, APP_NAME, nullptr, nullptr);
    if (window == nullptr)
        return;
    
    // Setup Menubar
    auto item = [[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength];
    [[item button] setTitle:@"🎧"];
    [[item button] setTarget:[StatusBarButtonCallback class]];
    [[item button] setAction:@selector(buttonAction:)];

    id <MTLDevice> device = MTLCreateSystemDefaultDevice();
    id <MTLCommandQueue> commandQueue = [device newCommandQueue];

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplMetal_Init(device);

    NSWindow *nswin = glfwGetCocoaWindow(window);

    CAMetalLayer *layer = [CAMetalLayer layer];
    layer.device = device;
    layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    nswin.contentView.layer = layer;
    nswin.contentView.wantsLayer = YES;

    MTLRenderPassDescriptor *renderPassDescriptor = [MTLRenderPassDescriptor new];

    // Main loop
    {
        // Save config to user's Library/Preferences
        std::string configPath = std::string(getenv("HOME")) + "/Library/Preferences/" + APP_CONFIG_NAME;
        App app(std::move(bt), configPath);
        while (!glfwWindowShouldClose(window)) {
            if (!clickHandled){
                if ([nswin isVisible]){
                    [nswin orderOut: nswin];
                } else {
                    [nswin makeKeyAndOrderFront: nswin];
                }
                clickHandled = true;
            }
            @autoreleasepool {
                // Poll and handle events (inputs, window resize, etc.)
                // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
                // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
                // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
                // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
                glfwPollEvents();

                int width, height;
                glfwGetFramebufferSize(window, &width, &height);
                layer.drawableSize = CGSizeMake(width, height);
                id <CAMetalDrawable> drawable = [layer nextDrawable];

                id <MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
                renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 0);
                renderPassDescriptor.colorAttachments[0].texture = drawable.texture;
                renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
                renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
                id <MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];

                // Start the Dear ImGui frame
                ImGui_ImplMetal_NewFrame(renderPassDescriptor);
                ImGui_ImplGlfw_NewFrame();

                ImGui::NewFrame();
                // Our GUI routine
                app.OnImGui();
                ImGui::EndFrame();

                // Rendering
                ImGui::Render();
                ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), commandBuffer, renderEncoder);

                [renderEncoder endEncoding];

                [commandBuffer presentDrawable:drawable];
                [commandBuffer commit];
            }
        }
    }
    // Cleanup
    ImGui_ImplMetal_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

int main(){
    try
    {
        EnterGUIMainLoop(BluetoothWrapper(std::make_unique<MacOSBluetoothConnector>()));
    }
    catch (const std::exception &e)
    {
        printf("%s", e.what());
        return 1;
    }
    return 0;
}
