#include "SPH.h"

SPH::SPH()
{
    //Initialize ssbo buffer vectors
    GRID_RESOLUTION = glm::ivec3(500, 200, 500);
    GRID_SIZE = GRID_RESOLUTION.x * GRID_RESOLUTION.y * GRID_RESOLUTION.z;
    particles         = ParticleSystem(PARTICLE_COUNT);
    color             = std::vector<glm::vec4>(PARTICLE_COUNT);
    cellParticleIndex = std::vector<unsigned int>(PARTICLE_COUNT);
    gridCellCount     = std::vector<unsigned int>(GRID_SIZE);
    gridCellOffset    = std::vector<unsigned int>(GRID_SIZE);
    cellOffsetCount   = 0;
    grid_spacing = 1.0f/(H);

    //Initialize buffers
    glGenBuffers(1, &positionBuffer);
    glGenBuffers(1, &velocityBuffer);
    glGenBuffers(1, &forceBuffer);
    glGenBuffers(1, &densityBuffer);
    glGenBuffers(1, &pressureBuffer);
    glGenBuffers(1, &colorBuffer);
    glGenBuffers(1, &gridCountBuffer);
    glGenBuffers(1, &gridOffsetBuffer);
    glGenBuffers(1, &cellOffsetBuffer);
    glGenBuffers(1, &cellParticleBuffer);


    initDamBreak();
    initSPHBasic();
    initComputeShaders();
}

void SPH::initComputeShaders() {
    //SPH Buffers
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, positionBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, PARTICLE_COUNT * sizeof(glm::vec4), &particles.position[0], GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velocityBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, PARTICLE_COUNT * sizeof(glm::vec4), &particles.velocity[0], GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, forceBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, PARTICLE_COUNT * sizeof(glm::vec4), &particles.force[0], GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, densityBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, PARTICLE_COUNT * sizeof(float), &particles.density[0], GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, pressureBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, PARTICLE_COUNT * sizeof(float), &particles.pressure[0], GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, colorBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, PARTICLE_COUNT * sizeof(glm::vec4), &color[0], GL_DYNAMIC_DRAW);

    //Nearest Neighbor Buffers
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, gridCountBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, GRID_SIZE * sizeof(unsigned int), &gridCellCount[0], GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, gridOffsetBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, GRID_SIZE * sizeof(unsigned int), &gridCellOffset[0], GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, cellOffsetBuffer);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(unsigned int), &cellOffsetCount, GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, cellParticleBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, PARTICLE_COUNT * sizeof(unsigned int), &cellParticleIndex[0], GL_DYNAMIC_DRAW);
}

void SPH::initSPHWicked() {
    ComputeShader  compDensityPressure = ComputeShader("src/shader/sph/Wicked/wicked_comp_density.comp");
    ComputeShader  compForces = ComputeShader("src/shader/sph/Wicked/wicked_comp_force.comp");



    compCellPartitioning.use();
    compCellPartitioning.setIVec3("grid_resolution", GRID_RESOLUTION);
    compCellPartitioning.setFloat("grid_spacing", grid_spacing);

    compCellAllocation.use();
    compCellAllocation.setIVec3("grid_resolution", GRID_RESOLUTION);


    compCellBinning.use();
    compCellBinning.setFloat("grid_spacing", grid_spacing);
    compCellBinning.setIVec3("grid_resolution", GRID_RESOLUTION);


    compDensityPressure.use();
    compForces.setFloat("H", H);
    compDensityPressure.setIVec3("grid_resolution", GRID_RESOLUTION);
    compDensityPressure.setFloat("grid_spacing", grid_spacing);
    compDensityPressure.setInt("size_of_extern", 0);
    compDensityPressure.setInt("num_of_particles", PARTICLE_COUNT);
    compDensityPressure.setFloat("HSQ", HSQ);
    compDensityPressure.setFloat("MASS", MASS);
    compDensityPressure.setFloat("KERNEL", KERNEL_MULLER);
    compDensityPressure.setFloat("GAS_CONSTANT", K);
    compDensityPressure.setFloat("REST_DENSITY", REST_DENSITY);

    compForces.use();
    compForces.setIVec3("grid_resolution", GRID_RESOLUTION);
    compForces.setFloat("grid_spacing", grid_spacing);
    compForces.setInt("num_of_particles", PARTICLE_COUNT);
    compForces.setFloat("H", H);
    compForces.setFloat("MASS", MASS);
    compForces.setFloat("KERNEL_GRAD", SPIKY_GRAD);
    compForces.setFloat("VISC_LAP", VISC_LAP);
    compForces.setFloat("G", G);
    compForces.setFloat("VISC", VISC);
    compForces.setFloat("GAS_CONSTANT", K);
    compForces.setFloat("REST_DENSITY", REST_DENSITY);

    compIntegration.use();
    compIntegration.setBool("hasExtern", false);
    compIntegration.setFloat("DT", DT);
    compIntegration.setFloat("radius", EPS);
    compIntegration.setFloat("RESTITUTION_COEFFICIENT", RESTITUTION_COEFFICIENT);
    compIntegration.setVec3("BOUND_BOX_POS", box.position);
    compIntegration.setVec3("BOUND_BOX_SCALE", box.scale);

}


void SPH::initSPHBasic() {
    compCellPartitioning.use();
    compCellPartitioning.setIVec3("grid_resolution", GRID_RESOLUTION);
    compCellPartitioning.setFloat("grid_spacing", grid_spacing);

    compCellAllocation.use();
    compCellAllocation.setIVec3("grid_resolution", GRID_RESOLUTION);


    compCellBinning.use();
    compCellBinning.setFloat("grid_spacing", grid_spacing);
    compCellBinning.setIVec3("grid_resolution", GRID_RESOLUTION);


    compDensityPressure.use();
    compForces.setFloat("H", H);
    compDensityPressure.setIVec3("grid_resolution", GRID_RESOLUTION);
    compDensityPressure.setFloat("grid_spacing", grid_spacing);
    compDensityPressure.setInt("size_of_extern", 0);
    compDensityPressure.setInt("num_of_particles", PARTICLE_COUNT);
    compDensityPressure.setFloat("HSQ", HSQ);
    compDensityPressure.setFloat("MASS", MASS);
    compDensityPressure.setFloat("KERNEL", POLY6);
    compDensityPressure.setFloat("GAS_CONSTANT", K);
    compDensityPressure.setFloat("REST_DENSITY", REST_DENSITY);

    compForces.use();
    compForces.setIVec3("grid_resolution", GRID_RESOLUTION);
    compForces.setFloat("grid_spacing", grid_spacing);
    compForces.setInt("num_of_particles", PARTICLE_COUNT);
    compForces.setFloat("H", H);
    compForces.setFloat("MASS", MASS);
    compForces.setFloat("KERNEL_GRAD", SPIKY_GRAD);
    compForces.setFloat("VISC_LAP", VISC_LAP);
    compForces.setFloat("G", G);
    compForces.setFloat("VISC", VISC);

    compIntegration.use();
    compIntegration.setBool("hasExtern", false);
    compIntegration.setFloat("DT", DT);
    compIntegration.setFloat("radius", EPS);
    compIntegration.setFloat("RESTITUTION_COEFFICIENT", RESTITUTION_COEFFICIENT);
    compIntegration.setVec3("BOUND_BOX_POS", box.position);
    compIntegration.setVec3("BOUND_BOX_SCALE", box.scale);

}

void SPH::updateShaderUniforms() {
    compDensityPressure.use();
    compForces.setFloat("H", H);
    compDensityPressure.setIVec3("grid_resolution", GRID_RESOLUTION);
    compDensityPressure.setFloat("grid_spacing", grid_spacing);
    compDensityPressure.setInt("size_of_extern", 0);
    compDensityPressure.setInt("num_of_particles", PARTICLE_COUNT);
    compDensityPressure.setFloat("HSQ", HSQ);
    compDensityPressure.setFloat("MASS", MASS);
    compDensityPressure.setFloat("KERNEL", POLY6);
    compDensityPressure.setFloat("GAS_CONSTANT", K);
    compDensityPressure.setFloat("REST_DENSITY", REST_DENSITY);

    compForces.use();
    compForces.setIVec3("grid_resolution", GRID_RESOLUTION);
    compForces.setFloat("grid_spacing", grid_spacing);
    compForces.setInt("num_of_particles", PARTICLE_COUNT);
    compForces.setFloat("H", H);
    compForces.setFloat("MASS", MASS);
    compForces.setFloat("KERNEL_GRAD", SPIKY_GRAD);
    compForces.setFloat("VISC_LAP", VISC_LAP);
    compForces.setFloat("G", G);
    compForces.setFloat("VISC", VISC);

    compIntegration.use();
    compIntegration.setBool("hasExtern", false);
    compIntegration.setFloat("DT", DT);
    compIntegration.setFloat("radius", EPS);
    compIntegration.setFloat("RESTITUTION_COEFFICIENT", RESTITUTION_COEFFICIENT);
    compIntegration.setVec3("BOUND_BOX_POS", box.position);
    compIntegration.setVec3("BOUND_BOX_SCALE", box.scale);
}


void SPH::addExternalObject(BoundingBox& externalObj) {
    compIntegration.use();
    compIntegration.setBool("hasExtern", true);
    compIntegration.setVec3("EXTERN_BOX_POS", externalObj.position);
    compIntegration.setVec3("EXTERN_BOX_SCALE", externalObj.scale);
}

void SPH::removeExternalObjects() {
    compIntegration.setBool("hasExtern", false);
}

void SPH::clearNeighborData() {
    unsigned int zero = 0;

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, gridCountBuffer);
    glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, &zero);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, gridOffsetBuffer);
    glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, &zero);

    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, cellOffsetBuffer);
    glClearBufferData(GL_ATOMIC_COUNTER_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, &zero);


    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, cellParticleBuffer);
    glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, &zero);
}

void SPH::update()
{
    compCellPartitioning.use();
    glDispatchCompute(particles.size() / 1536, 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glUseProgram(0);

    compCellAllocation.use();
    glDispatchCompute(GRID_SIZE / 1000, 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glUseProgram(0);

    compCellBinning.use();
    glDispatchCompute(particles.size() / 1536, 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glUseProgram(0);

    //SPH
    compDensityPressure.use();
    glDispatchCompute(particles.size()/ 1536, 1, 1); 
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glUseProgram(0);

    compForces.use(); 
    glDispatchCompute(particles.size()/ 1536, 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glUseProgram(0);

    compIntegration.use();
    glDispatchCompute(particles.size()/ 1536, 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glUseProgram(0);

    clearNeighborData();
}

void SPH::reset(int scene) {
    particles.reset();
    PARTICLE_COUNT = 1536 * PARTICLE_COUNT_MULTIPLIER;
    particles = ParticleSystem(PARTICLE_COUNT);
    color = std::vector<glm::vec4>(PARTICLE_COUNT);
    cellParticleIndex = std::vector<unsigned int>(PARTICLE_COUNT);
    gridCellCount = std::vector<unsigned int>(GRID_SIZE);
    gridCellOffset = std::vector<unsigned int>(GRID_SIZE);
    cellOffsetCount = 0;
    grid_spacing = 1.0f / (H);

    if (scene != 1) {
        particles.slam = false;
    }
    else {
        particles.slam = true;
    }
    initDamBreak();
    initComputeShaders();
}

void SPH::initDamBreak() {
    glm::vec3& boxPos = box.position;
    glm::vec3& boxScale = box.scale;
    bool switchMade = false;
    for (float y = boxPos.y; y < boxPos.y + (boxScale.y*40000); y += H) {
        //if (particles.size() >= (0.5f * PARTICLE_COUNT) && !switchMade) { y += 1000; switchMade = true; }
        for (float z = boxPos.z - (boxScale.z/4); z < boxPos.z + (boxScale.z / 4); z += H) {
            for (float x = boxPos.x - (boxScale.x/4); x < boxPos.x + (boxScale.x/4 ); x += H) {
                if (particles.size() < PARTICLE_COUNT) {
                    float jitter = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
                    float x_val = x + jitter;
                    float z_val = z + jitter;
                    float y_val = y;
                    assert(!isnan(x_val));
                    assert(!isnan(z_val));
                    assert(!isnan(y_val));
                    particles.add_particle(glm::vec4(x_val, y_val, z_val, 1.0));
                }
            }
        }
    }
    PARTICLE_COUNT = particles.size();
}



void SPH::updateSmoothingRadius(float newH)
{
    H = newH;
    HSQ = H * H;
    HCB = HSQ * H;
    H4 = HSQ * HSQ;
    H5 = H4 * H;
}



std::vector<glm::vec4>& SPH::getPositions()
{
    return particles.position;
}

std::vector<float>& SPH::getDensities()
{
    return particles.density;
}

