#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;

layout(push_constant) uniform Push{
	mat2 transform;
	vec2 offset;
	vec3 color;
}push;

void main(){
	vec2 transformedPosition = push.transform * position + push.offset; 
	gl_Position = vec4(transformedPosition,0.0,1.0);
}