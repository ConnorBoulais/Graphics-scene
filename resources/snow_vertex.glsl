#version 330 core
layout(location = 0) in vec3 vertPos;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform float t;
uniform float r;

out vec3 vertex_pos;
void main()
{
	vertex_pos = vertPos;
	vertex_pos.x += r * cos(t);
	vertex_pos.y += r * sin(t);
	gl_PointSize = 5.0;
	gl_Position = M * vec4(vertex_pos, 1.0);
}
