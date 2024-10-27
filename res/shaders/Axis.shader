#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 ourColor;

uniform mat4 u_MVP;

void main() {
    gl_Position = u_MVP * vec4(aPos, 1.0);
    ourColor = aColor;
}

#shader fragment
#version 330 core

in vec3 ourColor;
out vec4 color;

void main() {
    color = vec4(ourColor, 1.0);
}