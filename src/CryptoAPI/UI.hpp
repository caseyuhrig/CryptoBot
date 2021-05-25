#pragma once


namespace CryptoAPI {

    static GLFWwindow* m_Window;
    static ImVec4 clear_color;

    class UI
    {
    public:


        UI() {}

        static void glfw_error_callback(int error, const char* description)
        {
            spdlog::critical("Glfw Error {}: {}", error, description);
        }

        static void Initialize()
        {
            spdlog::debug("UI::Initialize()");

            //clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
            clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

            glfwSetErrorCallback(glfw_error_callback);
            if (!glfwInit())
                return;

            // Setup window


            // GL 3.0 + GLSL 130
            const char* glsl_version = "#version 450";
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
            //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            //glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
            //glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
            //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

            // Create window with graphics context
            m_Window = glfwCreateWindow(1598, 1198, "CryptoBot", NULL, NULL);
            if (m_Window == NULL)
                return;
            glfwMakeContextCurrent(m_Window);
            //glewInit();
            glfwSwapInterval(1); // Enable vsync

            //glfwInit();
            //bool err = gl3wInit() != 0;
            bool err = glewInit() != GLEW_OK;
            if (err)
            {
                spdlog::critical("Failed to initialize OpenGL loader!");
                return;
            }

            glfwSetWindowPos(m_Window, 1, 1);

            // Setup Dear ImGui context
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImPlot::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); 
            //(void)io;
            //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
            //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
            //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

            // Setup Dear ImGui style
            ImGui::StyleColorsDark();
            ImPlot::StyleColorsDark();
            ImPlot::GetStyle().UseLocalTime = true;
            ImPlot::GetStyle().Colormap = ImPlotColormap_Spectral;
            
            
            // Setup Platform/Renderer backends
            ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
            ImGui_ImplOpenGL3_Init(glsl_version);
        }

        static void ProcessEvents()
        {
            glfwPollEvents();
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }

        static void Render()
        {
            // Rendering
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(m_Window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(m_Window);
        }

        static bool Loop()
        {
            return !glfwWindowShouldClose(m_Window);
        }

        static void Shutdown()
        {
            // Cleanup
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImPlot::DestroyContext();
            ImGui::DestroyContext();

            glfwDestroyWindow(m_Window);
            glfwTerminate();
        }
    private:
    };
}
