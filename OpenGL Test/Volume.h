#pragma once


#include <iostream>
#include <vector>
#include <array>
#include<iterator> // for back_inserter
#include<algorithm> // for copy.
#include <bitset>
#include <cassert>  

using namespace std;

typedef unsigned int uint;

class VolumeCube {
private:
	bitset<8> _pointsInsideBitset;

public:
	VolumeCube(vector<int> pointsInside) {
		for (int i : pointsInside) {
			_pointsInsideBitset.set(i, 1);
		}
	}

	uint  getIndex() {
		return (int)_pointsInsideBitset.to_ulong();
	}

};

class Volume {

private:
	vector<vector<vector<float>>> volumeData;
	vector<vector<vector<VolumeCube*>>> volumeCubes;

	uint _sizeX;
	uint _sizeY;
	uint _sizeZ;
public:

	Volume(uint sizeX, uint sizeY, uint sizeZ) {
		_sizeX = sizeX;
		_sizeY = sizeY;
		_sizeZ = sizeZ;

		vector<float> xV(_sizeX);
		vector<vector<float>> xyV(_sizeY, xV);
		volumeData.resize(sizeZ, xyV);
		//vector<vector<vector<float>>>* xyzV = new vector<vector<vector<float>>> (sizeZ, yV);

		vector<VolumeCube*> volumeCubesX(_sizeX - 1, nullptr);
		vector<vector<VolumeCube*>> volumeCubesXY(_sizeY - 1, volumeCubesX);
		volumeCubes.resize(_sizeZ - 1, volumeCubesXY);
	}

	~Volume() {
		for (uint x = 0; x < _sizeX - 1; x++) {
			for (uint y = 0; y < _sizeY - 1; y++) {
				for (uint z = 0; z < _sizeZ - 1; z++) {
					if (volumeCubes[x][y][z] != nullptr) {
						delete(volumeCubes[x][y][z]);
						volumeCubes[x][y][z] = nullptr;
					}
				}
			}
		}
	}

	float getElement(uint x, uint y,uint z) {
		return volumeData[x][y][z];
	}

	void setElement(uint x, uint y, uint z, float value) {
		volumeData[x][y][z] = value;
	}

	VolumeCube getCube(uint x, uint y, uint z) {
		return *volumeCubes[x][y][z];
	}

	void computeCubes() {
		for (uint x = 0; x < _sizeX - 1; x++) {
			for (uint y = 0; y < _sizeY - 1; y++) {
				for (uint z = 0; z < _sizeZ - 1; z++) {

					vector<int> pointsInside;


					int X = x + 1, Y = y + 1, Z = z + 1;
					//Danger zone
					if (isPointThere(x, y, z)) { //000
						pointsInside.push_back(0);
					}
					if (isPointThere(x, y, Z)) { //001
						pointsInside.push_back(1);
					}
					if (isPointThere(x, Y, z)) { //010
						pointsInside.push_back(2);
					}
					if (isPointThere(x, Y, Z)) { //011
						pointsInside.push_back(3);
					}
					if (isPointThere(X, y, z)) { //100
						pointsInside.push_back(4);
					}
					if (isPointThere(X, y, Z)) { //101
						pointsInside.push_back(5);
					}
					if (isPointThere(X, Y, z)) {  //110
						pointsInside.push_back(6);
					}
					if (isPointThere(X, Y, z)) { //111
						pointsInside.push_back(7);
					}

					if (volumeCubes[x][y][z] != nullptr) {
						delete(volumeCubes[x][y][z]);
						volumeCubes[x][y][z] = nullptr;
					}
					VolumeCube *cube = new VolumeCube(pointsInside);

					volumeCubes[x][y][z] = cube;
				}
			}
		}
	}

private:
	bool isPointThere(uint x, uint y, uint z) {
		if (volumeData[x][y][z] > 0.5) {
			return true;
		}
		return false;
	}

};
