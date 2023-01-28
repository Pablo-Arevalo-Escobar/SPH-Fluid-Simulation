#pragma once
#include <vector>
#include <glm/glm.hpp>
class ParticleSystem
{
public:
    bool slam = false;
    ParticleSystem(int max_size);
    void add_particle(glm::vec4);
    int size();
    void reset();
    std::vector<glm::vec4> position;
    std::vector<glm::vec4> velocity;
    std::vector<glm::vec4> force;
    std::vector<float> density;
    std::vector<float> pressure;
private:
    int index = 0;
};
