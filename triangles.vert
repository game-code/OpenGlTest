// vexter
#version 430 core
layout (location = 0) in vec3 poistion;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 uvs;

out vec3 out_color;
out vec2 out_uv;
 
void main()
{
    gl_Position = vec4(poistion, 1.0);
    out_color = color;
    out_uv = uvs;
}