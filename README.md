# OpenGL Renderer
An OpenGL renderer using the [LearnOpenGL book by JoeyDeVries](https://learnopengl.com/) as a guide.

## Build
1. (Windows) Install the Visual Studio "Desktop development with C++" workload. Only the following components are necessary:
  - C++ Core Desktop Features
  - MSVC v{latest} - VS {version} C++ x64/x86 build tools
  - Windows {version} SDK

2. Install [xmake](https://xmake.io/).
3. (Optional) Configure with `xmake config -m {debug|release}`.
4. Compile with `xmake [run]`.

## Notes
- Use the `run` option while running xmake to immediately run the compiled executable.
- If you are using vscode, you can update the compile_commands.json with `xmake project -k compile_commands .vscode`, which will remove incorrectly detected error squiggles.

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
