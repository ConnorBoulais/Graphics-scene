#version 330 core
out vec4 color;
in vec3 vertex_color;
in vec3 vertex_normal;
in vec3 vertex_pos;

void main()
{
	vec3 normal = normalize(vertex_normal);
	vec3 light_pos = vec3(5, 10, 0);
	vec3 ld = normalize(light_pos - vertex_pos);
	float diffact = clamp(dot(normal, ld) + .4, 0, 1);

	color.r = normal.r ;//* diffact;
	color.g = normal.g ;//* diffact;
	color.b = normal.b ;//* diffact;
	color.a=1;	//transparency: 1 .. 100% NOT transparent
}
