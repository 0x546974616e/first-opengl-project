#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE
#include <optional> // std::optional{}

#include "Window.hpp"
#include "helper.hpp" // TR

int main(void) {
  auto window = TR::Window::Create();
  return window && window->MainLoop()
    ? EXIT_SUCCESS : EXIT_FAILURE;
}
