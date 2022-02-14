#version 460 core

in vec3 col;
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D ourTex;
uniform sampler2D nexTex;
uniform float ratio;

void main(){

FragColor = mix(texture(ourTex, vec2(-texCoord.s, texCoord.t)), texture(nexTex, texCoord), ratio);
}