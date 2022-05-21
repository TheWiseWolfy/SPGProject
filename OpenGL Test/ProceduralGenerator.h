#pragma once

#include <glm.hpp>
#include <vector>
#include <fstream> 

#include "Triangle.h"
#include "Volume.h"
#include "PerlinNoise.h"

class ProceduralGenerator {
private:
	Volume _volume;
	uint _sizeX, _sizeY, _sizeZ;
	float _granularity =4.0f;
	float _offset = -3.0f;
	PerlinNoise _noiseGenerator;

	void changeSeed();
	vector<Triangle> generateTerrainGeometry();
	static void generateCubeConfiguration();
	void generateVolume(glm::vec3 startPoint, glm::vec3 endPoint);

public:
	ProceduralGenerator(uint sizeX, uint sizeY, uint sizeZ): _volume(3 * sizeX, 3 * sizeY,3 * sizeZ){
		_sizeX = sizeX;
		_sizeY = sizeY;
		_sizeZ = sizeZ;

		_noiseGenerator = PerlinNoise((float)rand());
		// Create and open a text file

		generateCubeConfiguration();
	}
	vector<Triangle> GenerateAroundPlayer(glm::vec3 playerPoz);
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


 
