
# OpenGL based on SFML

- Use SFML library to open the window, instead of glew library.
- Use glad to load OpenGL functions.
- Use glm to do the math.
- Unlit rendering, forward light rendering & deferred light rendering.

## Base unlit rendering

![](./DocImages/BaseRendering.png)

## Forward light rendering

![](./DocImages/ForwardRendering.png)

## Deferred light rendering

![](./DocImages/DeferredRendering.png)

### GBuffer content

- Position buffer

![](./DocImages/GPos.png)

- Normal buffer

![](./DocImages/GNormal.png)

- Albedo & Specular

![](./DocImages/GAlbedoSpecular.png)