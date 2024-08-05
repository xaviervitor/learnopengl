# OpenGL Renderer
An OpenGL renderer using the [LearnOpenGL book by JoeyDeVries](https://learnopengl.com/) as a guide.
## Instructions
### Windows
1. Download [w64devkit](https://github.com/skeeto/w64devkit/releases), unzip and run ``w64devkit.exe``
2. Compile with `make [run] [build={debug|release}] [-j]`.

#### Notes
- Setting the `build` variable compiles with extra compiler flags (See Makefile `build_flags` variable).
- Running make with the `run` target compiles and immediately runs the generated executable.

## Demo
The pictures below show snapshots of this project's progress from newest to oldest.
<div float="center">
  <img width="32%" src="screenshots/13 depth stencil testing.png" alt="Depth and Stencil testing chapters screenshot" />
  <img width="32%" src="screenshots/12 model loading.png" alt="Model Loading section (Mesh & Model chapters) screenshot" />
  <img width="32%" src="screenshots/11 multiple lights.png" alt="Multiple Lights chapter screenshot" />
</div>
<div float="center">
  <img width="32%" src="screenshots/10 light casters.png" alt="Light Casters chapter screenshot" />
  <img width="32%" src="screenshots/9 lighting maps.png" alt="Lighting Maps chapter screenshot" />
  <img width="32%" src="screenshots/8 basic lighting.png" alt="Basic Lighting chapter screenshot" />
</div>
<div float="center">
  <img width="32%" src="screenshots/7 colors.png" alt="Colors chapter screenshot" />
  <img width="32%" src="screenshots/6 cameras.png" alt="Cameras chapter screenshot" />
  <img width="32%" src="screenshots/5 coordinate systems.png" alt="Coordinate Systems chapter screenshot" />
</div>
<div float="center">
  <img width="32%" src="screenshots/4 transformations.png" alt="Transformations chapter screenshot" />
  <img width="32%" src="screenshots/3 textures.png" alt="Textures chapter screenshot" />
  <img width="32%" src="screenshots/2 shaders.png" alt="Shaders chapter screenshot" />
</div>
<div float="center">
  <img width="32%" src="screenshots/1 hello triangle.png" alt="Hello Triangle chapter screenshot" />
  <img width="32%" src="screenshots/0 hello window.png" alt="Hello Window chapter screenshot" />
</div>
