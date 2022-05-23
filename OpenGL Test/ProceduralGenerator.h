#pragma once

#include <glm.hpp>
#include <vector>
#include <fstream> 

#include "Triangle.h"
#include "Volume.h"
#include "PerlinNoise.h"

class ProceduralGenerator {
private:
	Volume* _volumes[3][3][3];
	uint _sizeX, _sizeY, _sizeZ;
	uint _sizeCubesX, _sizeCubesY, _sizeCubesZ;

	glm::ivec3 _playerQuadrant;
	glm::ivec3 _oldPlayerQuadrant;
	glm::vec3 _drawingOffset;

	//Perlin noise generation
	float _granularity =16.0f;
	float _offset = -3.0f;
	PerlinNoise _noiseGenerator;

	void changeSeed();
	vector<Triangle>& convertVolumeToGeometry(int x, int y, int z);

	static void generateCubeConfiguration();
	Volume* createNewVolume(int offsetX, int offsetY, int offsetZ);
	void generateAllVolumes();

	//movement fuctions
	void  moveVolumeXPoz(int x);
	void  moveVolumeXNeg(int x);


public:
	ProceduralGenerator(uint sizeX, uint sizeY, uint sizeZ) {
		_sizeX = sizeX / 3;
		_sizeY = sizeY / 3;
		_sizeZ = sizeZ / 3;

		_sizeCubesX = _sizeX - 1;
		_sizeCubesY = _sizeX - 1;
		_sizeCubesZ = _sizeX - 1;

		_noiseGenerator = PerlinNoise((float)rand());
		// Create and open a text file
		_drawingOffset = glm::vec3(0,0,0);

		generateAllVolumes();
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


 
