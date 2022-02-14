#version 460 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec2 aTexCoord;

out vec3 col;
out vec2 texCoord;

uniform mat4 transMat;
uniform float time;

void main(){

gl_Position = transMat * vec4(aPos, 0.0, 1.0);
col = aCol;
texCoord = aTexCoord;
}