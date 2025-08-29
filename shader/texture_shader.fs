#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;


// uniform sampler2D ourTexture; // 采样器(sampler)
uniform sampler2D texture1; // 采样器(sampler)
uniform sampler2D texture2;

void main()
{
    // 混合两个纹理，0.8 * A + 0.2 * B
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
    // FragColor = texture(ourTexture, TexCoord);
}
