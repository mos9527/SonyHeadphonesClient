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

static void glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

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
        App app(std::move(bt));
        while (!glfwWindowShouldClose(window)) {
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
                [renderEncoder pushDebugGroup:@"ImGui demo"];

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

                [renderEncoder popDebugGroup];
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
    }
    return 1;
}