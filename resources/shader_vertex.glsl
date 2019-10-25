#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertColor;
layout(location = 2) in vec3 vertNor;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
out vec3 vertex_color;
out vec3 vertex_normal;
out vec3 vertex_pos;
void main()
{
	vertex_color = vertColor;
	vertex_normal = vec3(vertNor.x, vertNor.y, 0);
	vertex_pos = vertPos;
	gl_Position = P * V * M * vec4(vertex_pos, 1.0);
}
