#pragma once
#include <glm/glm.hpp>

class BoundingBox
{
public:
    glm::vec3 position;
    glm::vec3 scale;
    BoundingBox(glm::vec3 pos, glm::vec3 s) : position(pos), scale(s) {}
    void check_collision(glm::vec4& particle_pos, glm::vec4& particle_vel, float padding) {
        float restitution_coefficient = -0.99;
        if (particle_pos.y - padding < (position.y - (scale.y / 2))) {
            particle_pos.y = position.y - (scale.y / 2) + padding;
            particle_vel.y *= restitution_coefficient;
        }
        if (particle_pos.y + padding > (position.y + (scale.y / 2))) {
            particle_pos.y = position.y + (scale.y / 2) - padding;
            particle_vel.y *= restitution_coefficient;
        }
        if (particle_pos.x + padding > (position.x + (scale.x / 2))) {
            particle_pos.x = position.x + (scale.x / 2) - padding;
            particle_vel.x *= restitution_coefficient;
        }
        if (particle_pos.x - padding < (position.x - (scale.x / 2))) {
            particle_pos.x = position.x - (scale.x / 2) + padding;
            particle_vel.x *= restitution_coefficient;
        }
        if (particle_pos.z + padding > (position.z + (scale.z / 2))) {
            particle_pos.z = position.z + (scale.z / 2) - padding;
            particle_vel.z *= restitution_coefficient;
        }
        if (particle_pos.z - padding < (position.z - (scale.z / 2))) {
            particle_pos.z = position.z - (scale.z / 2) + padding;
            particle_vel.z *= restitution_coefficient;
        }

    }

};




