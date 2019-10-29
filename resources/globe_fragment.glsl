#version 330 core
out vec4 color;
in vec3 fragTex;
in vec3 fragNor;

uniform vec3 camera_pos;

void main()
{
	vec3 normal = normalize(fragNor);
	vec3 cam_pos = normalize(camera_pos);
	color.rgb = vec3(1,1,1);
	color.a = (1 - clamp(dot(normal, cam_pos),0,1));
}
