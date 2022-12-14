#version 330
out vec4 fragColor;
in vec2 mapping;
in vec4 color;

float near = 0.1;
float far = 1000.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));
}

uniform int viewMode;

void main()
{
//https://www.gamedev.net/blogs/entry/2249270-spherical-point-sprites-in-opengl-33/

    float lensqr = dot(mapping, mapping);
    if(lensqr > 1.0)
        discard;

    if(viewMode == 0) {
        fragColor = color;
    }
    else if(viewMode == 1) {
        vec3 N = vec3(mapping, sqrt(1.0-lensqr));
        fragColor =0.5* vec4(N.x+1, N.y+1, N.z+1,0.0);
    }
    else{
        float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
        fragColor = vec4(vec3(depth), 1.0);

    }
}    

  

//0.51,0.84,0.93