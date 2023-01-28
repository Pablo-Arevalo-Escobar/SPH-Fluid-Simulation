#include "ParticleSystem.h"
ParticleSystem::ParticleSystem(int max_size) :position(max_size), velocity(max_size), force(max_size), density(max_size), pressure(max_size),index(0)
{
    this->position.reserve(max_size);
    this->velocity.reserve(max_size);
    this->force.reserve(max_size);
    this->density.reserve(max_size);
    this->pressure.reserve(max_size);
}

void ParticleSystem::add_particle(glm::vec4 p)
{
    if (index < position.capacity())
    {
        position[index] = p;
        velocity[index] =  slam? glm::vec4(5000, 0, 0, 0): glm::vec4(0, 0, 0, 0);
        force[index] = glm::vec4(0, 0, 0, 0);
        density[index] = 0.f;
        pressure[index] = 0.f;
        index++;
    }
}

int ParticleSystem::size()
{
    return index;
}

void ParticleSystem::reset()
{
    index = 0;
}