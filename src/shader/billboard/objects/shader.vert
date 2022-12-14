#version 430
layout(std430,binding=6) buffer particlePos
{
    vec4 obj_pos[];
};

out vec4 color;
out vec2 mapping;
out float sphereRadius;

uniform mat4 view;
uniform mat4 projection;

uniform float BILLBOARD_SIZE = 0.5;

const vec3 BILLBOARD[] = {

  vec3(-1, -1, 0), // 0 bot left

  vec3(1, -1, 0), // 1 bot right

  vec3(-1, 1, 0), // 2 top left

  vec3(-1, 1, 0), // 3 top left

  vec3(1, -1, 0), // bot right

  vec3(1, 1, 0), // 5 top right

};

mat4 buildModel(vec3 delta)
{
    return mat4(
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(delta, 1.0));
}

void main()
{
    uint vertID = gl_VertexID%6;
    uint instanceID = gl_VertexID/6;
    vec3 quadPos = BILLBOARD[vertID];
    mapping = quadPos.xy;
    mat4 modelView = view*buildModel(obj_pos[instanceID].xyz);
    modelView[0].x =1 * BILLBOARD_SIZE; modelView[0].y = 0; modelView[0].z=0;
    modelView[1].x =0; modelView[1].y = 1 * BILLBOARD_SIZE; modelView[1].z=0;
    modelView[2].x =0; modelView[2].y = 0; modelView[2].z= 1 * BILLBOARD_SIZE;
    color = glm::vec4(0,0,0,0);
    gl_Position = projection*modelView*vec4(quadPos,1.0);
}