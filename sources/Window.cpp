#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <glad/glad.h> // OpenGL Loader
#include <GLFW/glfw3.h> // GLFW Library

#include <optional> // std::optional{}, std::nullopt
#include <utility> // std::in_place

#include "Event.hpp" // Self{}
#include "Window.hpp" // Self{}

#define TR_TITLE "[OpenGL] First Project"

static void ErrorCallback(int error, const char* description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
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
}

void Window::MouseCallback(GLFWwindow* window, double x, double y) NOEXCEPT {
  Window* source = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
  TR_ASSERT_RECOVERABLE(source != NULL);
  if (source == NULL) return;

  if (source->m_navigationMode) {
    MouseEvent event;
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

  glfwMaximizeWindow(window);
  glfwMakeContextCurrent(window);
  // TODO: What is vertical synchronization?
  // glfwSwapInterval(1);

  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
    TR_ERROR("GLAD initialisation failed.");
    glfwTerminate();
    return std::nullopt;
  }

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
    // TODO: What about the mice for ImGui?
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
  static char const* const s_leftPanel = "LeftPanel";
  static char const* const s_rightPanel = "RightPannel";
  static char const* const s_bottomPanel = "BottomPanel";
  static char const* const s_centralPanel = "CentralPanel";
  static char const* const s_theme = "Theme";

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

    ImGuiID mainId = m_dockSpaceId;
    ImGuiID dockLeftId, dockRightId, dockBottomId, dockCentralId;
    ImGui::DockBuilderSplitNode(mainId, ImGuiDir_Right, 0.2f, &dockRightId, &mainId);
    ImGui::DockBuilderSplitNode(mainId, ImGuiDir_Down, 0.2f, &dockBottomId, &mainId);
    ImGui::DockBuilderSplitNode(mainId, ImGuiDir_Left, 0.2f, &dockLeftId, &dockCentralId);

    ImGui::DockBuilderDockWindow(s_leftPanel, dockLeftId);
    ImGui::DockBuilderDockWindow(s_rightPanel, dockRightId);
    ImGui::DockBuilderDockWindow(s_bottomPanel, dockBottomId);
    ImGui::DockBuilderDockWindow(s_centralPanel, dockCentralId);
    ImGui::DockBuilderDockWindow(s_theme, dockRightId);
    ImGui::DockBuilderFinish(m_dockSpaceId);
  }

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("#Window", NULL, windowFlags);
  ImGui::PopStyleVar(3);

  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Create")) { }
      if (ImGui::MenuItem("Open", "Ctrl+O")) { }
      if (ImGui::MenuItem("Save", "Ctrl+S")) { TR_DEBUG("dada"); }
      if (ImGui::MenuItem("Save as..")) { }
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }

  ImGui::DockSpace(m_dockSpaceId, ImVec2(0.0f, 0.0f), dockSpaceFlags);
  ImGui::End();

  static bool showDemoWindow = false;
  if (showDemoWindow) {
    ImGui::ShowDemoWindow();
  }

  if (ImGui::Begin(s_leftPanel)) {
    ImGui::Checkbox("Demo Window", &showDemoWindow);
  }
  ImGui::End();

  if (ImGui::Begin(s_bottomPanel)) {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text(
      "Average %.3f ms/frame (%.1f FPS)",
      1000.0f / io.Framerate, io.Framerate
    );
  }
  ImGui::End();

  if (ImGui::Begin(s_rightPanel)) {
    ImGui::Text("dada");
  }
  ImGui::End();
  m_theme.EditTheme(s_theme);

  {
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

    if (ImGui::Begin(s_centralPanel, NULL, ImGuiWindowFlags_NoBackground)) {
      if (ImGui::IsWindowFocused() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        if (!m_navigationMode) ToggleNavigationMode(true);
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

      ImGui::Begin("Example: Simple overlay", NULL, hintFlags);
      if (m_navigationMode) ImGui::Text("Press <Escape> to leave Navigation mode");
      else ImGui::Text("Press <i> or Double-Click to enter Navigation mode");
      ImGui::End();
    }
    ImGui::End();
  }
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
