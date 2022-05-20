#pragma once

#include <glm.hpp>
#include <vector>

#include "Triangle.h"
#include "Volume.h"
#include "PerlinNoise.h"

class ProceduralGenerator {
private:
	Volume _volume;
	uint _sizeX, _sizeY, _sizeZ;
	float _granularity = 8.0f;
	float _offset = 0.0f;
	PerlinNoise _noiseGenerator;

	void changeSeed();
	vector<Triangle> generateTerrainGeometry();

public:
	ProceduralGenerator(uint sizeX, uint sizeY, uint sizeZ): _volume(sizeX, sizeY, sizeZ){
		_sizeX = sizeX;
		_sizeY = sizeY;
		_sizeZ = sizeZ;
		_noiseGenerator = PerlinNoise((float)rand());
	}
	std::vector<Triangle> generateGeometry();
};


const glm::vec3 corners[8] = {
	{-0.5, -0.5, -0.5},
	{-0.5, 0.5, -0.5},
	{0.5, 0.5, -0.5},
	{0.5, -0.5, -0.5},

	{-0.5, -0.5, 0.5},
	{-0.5, 0.5, 0.5},
	{0.5, 0.5, 0.5},
	{0.5, -0.5, 0.5}
};


 
