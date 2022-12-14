#version 330
out vec4 fragColor;
in vec2 mapping;
in vec4 color;

void main()
{
    float lensqr = dot(mapping, mapping);
    if(lensqr > 1.0)
        discard;
    fragColor = color;
}

//0.51,0.84,0.93