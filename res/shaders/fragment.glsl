#version 400
out vec4 FragColor;

in vec2 TexCoord;

// Texturas
uniform sampler2D texture1;

void main() {
    FragColor = texture(texture1, TexCoord);
}