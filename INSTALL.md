
# Install OpenGL

( Debian GNU/Linux 12 (bookworm) x86_64 )

<!-- -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -->

## GLFW3 (Graphics Library Framework)

Install library:

```sh
apt-get install pkg-config libglfw3-dev
```

Find library:

```sh
pkg-config --libs glfw3
ldconfig --print | grep -i glfw
dpkg --listfiles libglfw3-dev
dpkg --listfiles libglfw3
```

## GLAD

Web service <https://glad.dav1d.de/>

- Language: C/C++
- Specification: OpenGL
- Api:
  - gl: Version 4.6
  - gles1: None
  - gles2: None
  - gles3: None
- Profile: Core
- Options:
  - [X] Generate a loader
  - [ ] Omit KHR
  - [ ] Local Files

```txt
glad.zip
├─ include/
│  ├─ KHR/
│  │  └─ khrplatform.h
│  └─ glad/
│     └─ glah.h
└─ src/
   └─ glad.c
```

## Dear ImGui

Retrieve the following files from <https://github.com/ocornut/imgui/tree/docking>:

- `imconfig.h`
- `imgui.cpp`
- `imgui_demo.cpp`
- `imgui-docking.zip`
- `imgui_draw.cpp`
- `imgui.h`
- `imgui_internal.h`
- `imgui_tables.cpp`
- `imgui_widgets.cpp`
- `imstb_rectpack.h`
- `imstb_textedit.h`
- `imstb_truetype.h`

With backends:

- `imgui_impl_glfw.cpp`
- `imgui_impl_glfw.h`
- `imgui_impl_opengl3.cpp`
- `imgui_impl_opengl3.h`
- `imgui_impl_opengl3_loader.h`

### Patch

File `imgui.cpp` line `18206`

(source: <https://github.com/ocornut/imgui/issues/5634>)

```cpp
// Draw whole dockspace background if ImGuiDockNodeFlags_PassthruCentralNode if
// set.  We need to draw a background at the root level if requested by
// ImGuiDockNodeFlags_PassthruCentralNode, but we will only know the correct
// pos/size _after_ processing the resizing splitters. So we are using the
// DrawList channel splitting facility to submit drawing primitives out of
// order!
const bool render_dockspace_bg = node->IsRootNode()
  && host_window && (node_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0;
if (false && render_dockspace_bg && node->IsVisible) { // TR
    host_window->DrawList->ChannelsSetCurrent(DOCKING_HOST_DRAW_CHANNEL_BG);
    if (central_node_hole) [...]
}
```
