
# Install OpenGL

## Debian GNU/Linux 12 (bookworm) x86_64

<!-- -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -->

```sh
apt-get update
apt-get install cmake pkg-config
apt-get install libglew-dev libglfw3-dev libglm-dev
```

### GLFW3 (Graphics Library Framework)

```sh
pkg-config --libs glfw3
ldconfig --print | grep -i glfw
dpkg --listfiles libglfw3-dev
dpkg --listfiles libglfw3
```

### GLAD

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
