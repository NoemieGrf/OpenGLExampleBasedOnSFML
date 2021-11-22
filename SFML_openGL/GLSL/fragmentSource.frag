#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform float fragIn;
uniform sampler2D ourTexture;
uniform sampler2D ourFace;

void main()
{
    FragColor = mix(texture(ourTexture, TexCoord), texture(ourFace, TexCoord), 0.5);
	FragColor = mix(FragColor, vec4(ourColor, 1.0), 0.5);
	FragColor = vec4(FragColor.x, FragColor.y * fragIn, FragColor.z, FragColor.w);
}