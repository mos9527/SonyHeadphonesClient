#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "Constants.h"
#include "App.h"
#include "LinuxBluetoothConnector.h"

static void glfw_error_callback(int error, const char *description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void EnterGUIMainLoop(BluetoothWrapper bt)
{
    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit())
    {
        printf("Glfw init error. Quitting.");
        return;
    }


    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    // see https://github.com/glfw/glfw/commit/a9cc7c7260c32068a5374ce9d59515df540de970
    // Introduced in GLFW 3.4, GLFW_SCALE_FRAMEBUFFER is defaulted to true.
    // glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_TRUE);

    // Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(GUI_WIDTH, GUI_HEIGHT, APP_NAME, NULL, NULL);
    if (window == NULL)
        return;

    // init GUI    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    ImGuiIO &io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    // Main loop
    {
        App app = App(std::move(bt));    
        while (!glfwWindowShouldClose(window))
        {

            glfwPollEvents();

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();

            ImGui::NewFrame();
            // Our GUI routine
            app.OnImGui();
            ImGui::EndFrame();
            
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }
    }
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
};

int main(int argc, const char *argv[])
{
    try
    {
        EnterGUIMainLoop(BluetoothWrapper(std::make_unique<LinuxBluetoothConnector>()));
    }
    catch (const std::exception &e)
    {
        printf("%s", e.what());
        return 1;
    }
    return 0;
}