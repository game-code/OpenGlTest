// fragment
#version 430 core

// Ouput data
out vec4 color;

in vec3 out_color;
in vec2 out_uv;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform sampler2D ourTexture;

void main()
{
    vec2 uvs = out_uv;
    color = texture(texture2, uvs);
    //color = mix(texture(texture1, uvs), texture(texture2, uvs), 0.2);
    //color = vec4(out_color, 1.);
    //glFragColor = vec4(out_color,1.);
}