#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "ImGuiCustom.hpp" // operator+(ImVec2)

#include <glad/glad.h> // OpenGL Loader
#include <GLFW/glfw3.h> // GLFW Library

#include <optional> // std::optional{}, std::nullopt
#include <utility> // std::in_place

#include "Event.hpp" // Event{}
#include "Window.hpp" // Window{}
#include "Log.hpp" // TR_ERROR(), GlobalLog(), GlobalLogRender()
#include "helper.hpp" // NOEXCEPT

#define TR_TITLE "[OpenGL] First Project"

static void ErrorCallback(int error, const char* description) {
  GlobalLog(stderr, "GLFW Error %d: %s\n", error, description);
}

void Window::KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) NOEXCEPT {
  Window* source = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
  TR_ASSERT_RECOVERABLE(source != NULL);
  if (source == NULL) return;

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    if (source->m_navigationMode) source->ToggleNavigationMode(false);
    else glfwSetWindowShouldClose(window, true);
  }
  else if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
    if (!source->m_navigationMode) source->ToggleNavigationMode(true);
  }

  if (source->m_navigationMode && key == GLFW_KEY_H && action == GLFW_PRESS) {
    GLint polygonMode[2]; // .[1] = GL_FILL | GL_LINE | GL_POINT
    glGetIntegerv(GL_POLYGON_MODE, polygonMode);
    // GL_FRONT_AND_BACK: apply it to the front and back of all triangles.
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode[1] == GL_FILL ? GL_LINE : GL_FILL);
  }
}

void Window::MouseCallback(GLFWwindow* window, double x, double y) NOEXCEPT {
  Window* source = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
  TR_ASSERT_RECOVERABLE(source != NULL);
  if (source == NULL) return;

  if (source->m_navigationMode) {
    MouseEvent event;
    // TODO: Current/Elapsed per event.
    double currentTime = glfwGetTime();
    event.currentTime = currentTime,
    event.elapsedTime = currentTime - source->m_currentTime,
    event.x = x; event.y = y;
    source->m_engine.ProcessMouse(event);
  }
}

void Window::ScrollCallback(GLFWwindow* window, double x, double y) NOEXCEPT {
  Window* source = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
  TR_ASSERT_RECOVERABLE(source != NULL);
  if (source == NULL) return;

  if (source->m_navigationMode) {
    ScrollEvent event;
    // TODO: Current/Elapsed per event.
    double currentTime = glfwGetTime();
    event.currentTime = currentTime,
    event.elapsedTime = currentTime - source->m_currentTime,
    event.xOffset = x; event.yOffset = y;
    source->m_engine.ProcessScroll(event);
  }
}

std::optional<Window> Window::Create(void) NOEXCEPT {
  glfwSetErrorCallback(ErrorCallback);
  if (glfwInit() == GLFW_FALSE) {
    TR_ERROR("GLFW initialisation failed.");
    return std::nullopt;
  }

  #define TR_GLSL_VERSION "#version 330"
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // #if __APPLE__ ?
  glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

  GLFWwindow* window = glfwCreateWindow(1280, 720, TR_TITLE, NULL, NULL);
  if (window == NULL) {
    TR_ERROR("GLFW window creation failed.");
    glfwTerminate();
    return std::nullopt;
  }

  TR_DEBUG("GLFW initialised.");
  glfwMaximizeWindow(window);
  glfwMakeContextCurrent(window);
  // TODO: What is vertical synchronization?
  // glfwSwapInterval(1);

  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
    TR_ERROR("GLAD initialisation failed.");
    glfwTerminate();
    return std::nullopt;
  }

  TR_DEBUG("GLAD initialised.");
  glEnable(GL_BLEND);
  // FinalColor = FragColor.rgb * FragColor.a + FrameBuffer.rgb * (1 - FragColor.a)
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  TR_DEBUG(
    "OpenGL Version %d.%d"
    , GLVersion.major
    , GLVersion.minor
  );

  return std::optional<Window>(std::in_place, window);
}

Window::Window(GLFWwindow* window) NOEXCEPT
  : m_window(window), m_dockSpaceId(0), m_engine(), m_theme()
{
  glfwSetWindowUserPointer(window, this);
  glfwSetKeyCallback(window, KeyboardCallback);
  glfwSetCursorPosCallback(window, MouseCallback);
  glfwSetScrollCallback(window, ScrollCallback);

  float xscale = 0.5, yscale = 0; // TODO: TMP
  glfwGetWindowContentScale(window, &xscale, &yscale);
  TR_DEBUG("Window Scaling Factor: %.1f, %.1f", xscale, yscale);

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void) io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigDockingWithShift = false;
  io.IniFilename = NULL;
  io.LogFilename = NULL;

  ImGui::StyleColorsDark();
  ImGuiStyle& style = ImGui::GetStyle(); (void) style;
  style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  m_theme.ApplyTheme();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(TR_GLSL_VERSION);
  TR_DEBUG("ImGui initialised.");
  TR_ERROR("Test error message");
}

Window::~Window(void) NOEXCEPT {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  TR_ASSERT_RECOVERABLE(m_window != NULL);
  glfwDestroyWindow(m_window);
  glfwTerminate();
  m_window = NULL;
}

bool Window::MainLoop(void) NOEXCEPT {
  TR_DEBUG("Start MainLoop.");
  while (!glfwWindowShouldClose(m_window)) {
    double currentTime = glfwGetTime();
    m_elapsedTime = currentTime - m_currentTime;
    m_currentTime = currentTime;

    if (glfwGetWindowAttrib(m_window, GLFW_ICONIFIED) != 0) {
      ImGui_ImplGlfw_Sleep(10);
      continue;
    }

    ProcessInput();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderUi();
    RenderEngine();
    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(m_window);
    glfwPollEvents();
  }

  return true;
}

void Window::ToggleNavigationMode(bool enter) NOEXCEPT {
  if (enter && !m_navigationMode) {
    ImGui::SetWindowFocus(NULL);
    ImGuiIO io = ImGui::GetIO();
    // TODO: What about the mice for ImGui? (io.WantCaptureMouse)
    io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableGamepad;
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    m_navigationMode = true;
    m_engine.Focus();
  }
  else if (!enter && m_navigationMode) {
    ImGuiIO io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    m_navigationMode = false;
    m_engine.UnFocus();
  }
}

void Window::ProcessInput(void) NOEXCEPT {
  if (m_navigationMode) {
    KeyboardEvent event;
    // TODO: Current/Elapsed per event.
    event.currentTime = m_currentTime;
    event.elapsedTime = m_elapsedTime;
    event.keyA = glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS;
    event.keyD = glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS;
    event.keyS = glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS;
    event.keyW = glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS;
    event.shift = glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
    event.space = glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS;
    m_engine.ProcessKeyboard(event);
  }
}

void Window::RenderUi(void) NOEXCEPT {
  static char const* s_sceneTitle = "Scene";
  static char const* s_inspectorTitle = "Inspector";
  static char const* s_propertiesTitle = "Properties";
  static char const* s_themeTitle = "Theme";
  static char const* s_logTitle = "Logs";

  ImGuiDockNodeFlags dockSpaceFlags = (0
    | ImGuiDockNodeFlags_NoDockingInCentralNode
    | ImGuiDockNodeFlags_PassthruCentralNode
  );

  ImGuiWindowFlags windowFlags = (0
    | ImGuiWindowFlags_MenuBar
    | ImGuiWindowFlags_NoBackground
    | ImGuiWindowFlags_NoBringToFrontOnFocus
    | ImGuiWindowFlags_NoCollapse
    | ImGuiWindowFlags_NoDecoration
    | ImGuiWindowFlags_NoDocking
    | ImGuiWindowFlags_NoMove
    | ImGuiWindowFlags_NoNavFocus
    | ImGuiWindowFlags_NoResize
    | ImGuiWindowFlags_NoTitleBar
  );

  // - Important: Dockspaces need to be kept alive if hidden, otherwise windows
  //   docked into it will be undocked. e.g. if you have multiple tabs with a
  //   dockspace inside each tab: submit the non-visible dockspaces with
  //   ImGuiDockNodeFlags_KeepAliveOnly.
  //
  // if (dockSpaceFlags & ImGuiDockNodeFlags_KeepAliveOnly)
  //     host_windowFlags |= ImGuiWindowFlags_NoMouseInputs;

  m_dockSpaceId = ImGui::GetID("#DockSpace");
  ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::SetNextWindowSize(viewport->Size);
  ImGui::SetNextWindowPos(viewport->Pos);

  // Dockspaces need to be submitted before any window they can host.
  if (ImGui::DockBuilderGetNode(m_dockSpaceId) == NULL) {
    ImGui::DockBuilderRemoveNode(m_dockSpaceId);
    ImGui::DockBuilderAddNode(m_dockSpaceId, dockSpaceFlags | ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(m_dockSpaceId, viewport->Size);

    #if 0 // NOTE: TMP
      ImGuiID mainId = m_dockSpaceId;
      ImGuiID dockLeftId, dockRightId, dockBottomId, dockCentralId;
      ImGui::DockBuilderSplitNode(mainId, ImGuiDir_Right, 0.2f, &dockRightId, &mainId); // M|R
      ImGui::DockBuilderSplitNode(mainId, ImGuiDir_Down, 0.2f, &dockBottomId, &mainId); // M\B
      ImGui::DockBuilderSplitNode(mainId, ImGuiDir_Left, 0.2f, &dockLeftId, &dockCentralId); // L|C
      ImGui::DockBuilderDockWindow(s_sceneTitle, dockCentralId);
      ImGui::DockBuilderDockWindow(s_inspectorTitle, dockLeftId);
      ImGui::DockBuilderDockWindow(s_propertiesTitle, dockRightId);
      ImGui::DockBuilderDockWindow(s_themeTitle, dockRightId);
      ImGui::DockBuilderDockWindow(s_logTitle, dockBottomId);
    #else
      ImGuiID mainId = m_dockSpaceId;
      ImGuiID leftId, sceneId, logId, topId, bottomId;
      ImGui::DockBuilderSplitNode(mainId, ImGuiDir_Left, 0.2f, &leftId, &mainId); // L|M
      ImGui::DockBuilderSplitNode(mainId, ImGuiDir_Down, 0.225f, &logId, &sceneId); // S\l
      ImGui::DockBuilderSplitNode(leftId, ImGuiDir_Up, 0.7f, &topId, &bottomId); // T\B
      ImGui::DockBuilderDockWindow(s_sceneTitle, sceneId);
      ImGui::DockBuilderDockWindow(s_logTitle, logId);
      ImGui::DockBuilderDockWindow(s_themeTitle, topId);
      ImGui::DockBuilderDockWindow(s_propertiesTitle, topId);
      ImGui::DockBuilderDockWindow(s_inspectorTitle, bottomId);
    #endif

    ImGui::DockBuilderFinish(m_dockSpaceId);
  }

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("#Window", NULL, windowFlags);
  ImGui::PopStyleVar(3);
  DrawMenuBar();
  ImGui::DockSpace(m_dockSpaceId, ImVec2(0.0f, 0.0f), dockSpaceFlags);
  ImGui::End();

  DrawScene(s_sceneTitle);
  DrawInspector(s_inspectorTitle);
  DrawProperties(s_propertiesTitle);
  DrawTheme(s_themeTitle);
  DrawLogs(s_logTitle);
}

void Window::DrawMenuBar(void) NOEXCEPT {
  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Test Debug", "Ctrl+O")) { TR_DEBUG("Test Debug Message"); }
      if (ImGui::MenuItem("Test Error", "Ctrl+S")) { TR_ERROR("Test Error Message"); }
      ImGui::Separator();
      if (ImGui::MenuItem("Exit", "Escape")) {
        glfwSetWindowShouldClose(m_window, true);
      }
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
}

void Window::DrawScene(char const* title) NOEXCEPT {
  ImGuiWindowClass centralAlways;
  centralAlways.DockNodeFlagsOverrideSet |= (0
    | ImGuiDockNodeFlags_NoTabBar
    | ImGuiDockNodeFlags_NoDockingOverMe
    | ImGuiDockNodeFlags_NoUndocking
    | ImGuiDockNodeFlags_NoCloseButton
    | ImGuiDockNodeFlags_NoWindowMenuButton
  );

  ImGui::SetNextWindowClass(&centralAlways);
  ImGui::SetNextWindowDockID(m_dockSpaceId, ImGuiCond_Always);
  ImGui::SetNextWindowBgAlpha(0);

  if (ImGui::Begin(title, NULL, ImGuiWindowFlags_NoBackground)) {
    if (m_navigationMode) {
      if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        ToggleNavigationMode(false);
      }
    }
    else if (ImGui::IsWindowFocused() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
      ToggleNavigationMode(true);
    }

    ImGuiWindowFlags hintFlags = (0
      | ImGuiWindowFlags_AlwaysAutoResize
      | ImGuiWindowFlags_NoDecoration
      | ImGuiWindowFlags_NoDocking
      | ImGuiWindowFlags_NoFocusOnAppearing
      | ImGuiWindowFlags_NoMove
      | ImGuiWindowFlags_NoNav
      | ImGuiWindowFlags_NoSavedSettings
    );

    ImGui::SetNextWindowBgAlpha(0.35f);
    ImGui::SetNextWindowPos(
      ImGui::GetWindowPos()
      + ImGui::GetWindowSize()
      - ImGui::GetStyle().WindowPadding
      , ImGuiCond_Always
      , ImVec2(1.0f, 1.0f)
    );

    ImGui::Begin("Navigation Hint", NULL, hintFlags);
    if (m_navigationMode) ImGui::Text("Press <Escape> to leave Navigation mode");
    else ImGui::Text("Press <i> or <Double-Click> to enter Navigation mode");
    ImGui::End();
  }
  ImGui::End();
}

void Window::DrawInspector(char const* title) NOEXCEPT {
  static bool showDemoWindow = false;
  if (showDemoWindow) {
    ImGui::ShowDemoWindow(&showDemoWindow);
  }

  if (ImGui::Begin(title)) {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text(
      "Average %.3f ms/frame (%.1f FPS)",
      1000.0f / io.Framerate, io.Framerate
    );

    ImGui::Checkbox("Demo Window", &showDemoWindow);
  }
  ImGui::End();
}

void Window::DrawProperties(char const* title) NOEXCEPT {
  if (ImGui::Begin(title)) {
    m_engine.RenderUi();
  }
  ImGui::End();
}

void Window::DrawTheme(char const* title) NOEXCEPT {
  m_theme.EditTheme(title);
}

void Window::DrawLogs(char const* title) NOEXCEPT {
  GlobalLogRender(title);
}

void Window::RenderEngine(void) NOEXCEPT {
  GLint width, height, frameWidth, frameHeight;
  ImGuiDockNode* node = ImGui::DockBuilderGetCentralNode(m_dockSpaceId);
  glfwGetFramebufferSize(m_window, &frameWidth, &frameHeight);

  if (node == NULL) {
    width = frameWidth; height = frameHeight;
    glViewport(0, 0, frameWidth, frameHeight);
    m_engine.SetViewport(frameWidth, frameHeight);
  }
  else {
    width = static_cast<GLint>(node->Size.x);
    height = static_cast<GLint>(node->Size.y);
    GLint x = static_cast<GLint>(node->Pos.x);
    GLint y = static_cast<GLint>(node->Pos.y);
    glViewport(x, frameHeight - (y + height) , width, height);
    m_engine.SetViewport(width, height);
  }

  m_engine.Render({
    .currentTime = m_currentTime,
    .elapsedTime = m_elapsedTime,
  });
}
