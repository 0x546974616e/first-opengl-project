#include <cstdlib> // EXIT_SUCCESS, EXIT_FAILURE
#include <optional> // std::optional{}

#include "Window.hpp"

int main(void) {
  auto window = Window::Create();
  return window && window->MainLoop()
    ? EXIT_SUCCESS : EXIT_FAILURE;
}
