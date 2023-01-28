#pragma once
#include "ComputeShader.h"
#include "ParticleSystem.h"
#include "BoundingBox.h"
#include <glm/glm.hpp>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
// SPH class is soley responsible for the computations
// We let a different class perform all the rendering functionality
class SPH
{
private: 
	//Shaders
	ComputeShader  compCellPartitioning   = ComputeShader("src/shader/sph/neighbor/compute_partition_cells.comp");
	ComputeShader  compCellAllocation   = ComputeShader("src/shader/sph/neighbor/compute_cell_allocation.comp");
	ComputeShader  compCellBinning     = ComputeShader("src/shader/sph/neighbor/compute_cell_binning.comp");

    ComputeShader  compDensityPressure = ComputeShader("src/shader/sph/standard/compute_density_pressure.comp");
    ComputeShader  compForces		   = ComputeShader("src/shader/sph/standard/compute_forces.comp");
    ComputeShader  compIntegration     = ComputeShader("src/shader/sph/standard/compute_integration.comp");

	//OpenGl Buffer Data
	unsigned int  velocityBuffer, forceBuffer, densityBuffer, pressureBuffer;
	unsigned int  gridCountBuffer, gridOffsetBuffer, cellParticleBuffer, cellOffsetBuffer;
	std::vector<unsigned int> gridCellCount, gridCellOffset, cellParticleIndex;
	std::vector<glm::vec4> color;
	int cellOffsetCount;

	//Sim Data
    ParticleSystem particles = ParticleSystem(PARTICLE_COUNT);
	int GRID_SIZE;

	//File Data
	std::ofstream fileOut;
	std::ifstream fileIn;
	int frameCount = 1;
	int lineNumber = 0;
	void initComputeShaders();

public:
	unsigned int colorBuffer, positionBuffer, externalObjectsBuffer;
	std::vector<glm::vec4>& getPositions();
	std::vector<float>& getDensities();

	SPH();
	void addExternalObject(BoundingBox& b);
	void removeExternalObjects();
	void clearNeighborData();
	void update();
	void initDamBreak();

	void updateSmoothingRadius(float newRadius);
	void updateFromFile();
	void initSPHBasic();
	void initSPHWicked();
	void updateShaderUniforms();
	void reset(int scene);

	std::vector<std::vector<glm::vec4>>& readToVector(const char* fileName);



	//Simulation parameters
	BoundingBox box = BoundingBox(glm::vec3(0, 0, 0), glm::vec3(800, 150, 800));
	//BoundingBox box = BoundingBox(glm::vec3(0, 0, 0), glm::vec3(400, 150, 400));
	int PARTICLE_COUNT_MULTIPLIER = 40;
	unsigned int PARTICLE_COUNT = 1536 * PARTICLE_COUNT_MULTIPLIER;
	glm::ivec3 GRID_RESOLUTION;
	float G = -10.0f;
	float REST_DENSITY = 200.0f;
	float K = 2000.0f;
	float H = 16.0f;
	float HSQ = H * H;
	float HCB = HSQ * H;
	float H4 = HSQ * HSQ;
	float H5 = H4 * H;
	float MASS = 2.5f;
	float VISC = 250;
	float DT = 0.0007f;
	float EPS = H; 
	float RESTITUTION_COEFFICIENT = -0.5f;
	//Smoothing Kernels
	float KERNEL_MULLER = (315.0 / (64.0 * M_PI * HCB));
	float KERNEL_FIRST_DERIV = (-45.0 / (M_PI * H4));
	float KERNEL_SECOND_DERIV = (90.0 / (M_PI * H5));
	float POLY6 = 4.0f / (M_PI * pow(H, 8.f));
	float SPIKY_GRAD = -10.f / (M_PI * pow(H, 5.f));
	float VISC_LAP = 40.f / (M_PI * pow(H, 5.f));
	// particle rendering parameters
	float PARTICLE_SIZE = H / 2.0f;
	float grid_spacing;
};

