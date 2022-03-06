#version 460 core

layout (location = 0) in vec3 aPos;

out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj; //Short for projection

void main(){

gl_Position = proj * view * model * vec4(aPos, 1.0);
}