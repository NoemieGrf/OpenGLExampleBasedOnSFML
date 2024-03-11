
# OpenGL based on SFML

- Use SFML library to open the window, instead of glew library.
- Use glad to load OpenGL functions.
- Use glm to do the math.
- Unlit rendering, forward light rendering & deferred light rendering.

## Base unlit rendering

![](./DocImages/BaseRendering.gif)

## Forward light rendering

![](./DocImages/ForwardRendering.gif)

## Deferred light rendering

![](./DocImages/DeferredRendering.gif)

### GBuffer content

- Position buffer

![](./DocImages/GPos.gif)

- Normal buffer

![](./DocImages/GNormal.gif)

- Albedo & Specular

![](./DocImages/GAlbedoSpecular.gif)