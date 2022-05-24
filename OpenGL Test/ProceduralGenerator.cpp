
#include "ProceduralGenerator.h"
#include "LookUpTables.h"
#include "CubeConfiguration.h"

using namespace std;

//Private fuctions 
glm::vec3 avarageVec3(glm::vec3 polygon1, glm::vec3 polygon2);
std::vector<CubeConfiguration> cubeConfiguration(256);

//Generates 9 chunks around the player, when the player moves , 9 new chunks are generated in the right direction

void ProceduralGenerator::debugConsole() {
	cout << _playerQuadrant.x << " " << _playerQuadrant.y << " " << _playerQuadrant.z << "\n";
	cout << _gridLocation.x << " " << _gridLocation.y << " " << _gridLocation.z << "\n";;
}



vector<Triangle> ProceduralGenerator::GenerateAroundPlayer(glm::vec3 playerPoz) {

	_playerQuadrant.x = playerPoz.x / _sizeCubesX + 499;
	_playerQuadrant.y = playerPoz.y / _sizeCubesY + 499;
	_playerQuadrant.z = playerPoz.z / _sizeCubesZ + 499;

	glm::ivec3 positivePlayerQuadrant;
	positivePlayerQuadrant.x = playerPoz.x / _sizeCubesX + 499;
	positivePlayerQuadrant.y = playerPoz.y / _sizeCubesY + 499;
	positivePlayerQuadrant.z = playerPoz.z / _sizeCubesZ + 499;


	for (int f1 = 0; f1 < 3; f1++) {
		for (int f2 = 0; f2 < 3; f2++) {
			for (int f3 = 0; f3 < 3; f3++) {

				if (_playerQuadrant.x > _oldPlayerQuadrant.x) {
					moveVolumeXPoz();
				}
				else if (_playerQuadrant.x < _oldPlayerQuadrant.x) {
					moveVolumeXNeg();
				}
				 if (_playerQuadrant.y > _oldPlayerQuadrant.y) {
					moveVolumeYPoz();
				}
				else if (_playerQuadrant.y < _oldPlayerQuadrant.y) {
					moveVolumeYNeg();
				}
				 if (_playerQuadrant.z > _oldPlayerQuadrant.z) {
					moveVolumeZPoz();
				}
				else if (_playerQuadrant.z < _oldPlayerQuadrant.z) {
					moveVolumeZNeg();
				}

				_drawingOffset.x = _gridLocation.x * (int)_sizeCubesX - (int)_sizeCubesX;
				_drawingOffset.y = _gridLocation.y * (int)_sizeCubesY - (int)_sizeCubesY;
				_drawingOffset.z = _gridLocation.z * (int)_sizeCubesZ - (int)_sizeCubesZ;

				_oldPlayerQuadrant = _playerQuadrant;
			}
		}
	}

	

	//We need this to notice if the player swicher quadrands

	//Gather all geometry

	vector<Triangle> trianglesToDraw;

	for (int fx = 0; fx < 3; fx++) {
		for (int fy = 0; fy < 3; fy++) {
			for (int fz = 0; fz < 3; fz++) {
				auto volumeGeometry = convertVolumeToGeometry(fx, fy, fz);
				trianglesToDraw.insert(trianglesToDraw.end(), volumeGeometry->begin(), volumeGeometry->end());
			}
		}
	}
	return trianglesToDraw;
}

//Movement X
void  ProceduralGenerator::moveVolumeXPoz() {

	_gridLocation.x += 1;

	for (int fy = 0; fy < 3; fy++) {
		for (int fz = 0; fz < 3; fz++) {

			delete(_volumes[0][fy][fz]); //Aliberam memoria
			_volumes[0][fy][fz] = _volumes[1][fy][fz];
			_volumes[1][fy][fz] = _volumes[2][fy][fz];
			_volumes[2][fy][fz] = createNewVolume( _gridLocation.x +2, fy + _gridLocation.y, fz + _gridLocation.z);
		}
	}

}

void  ProceduralGenerator::moveVolumeXNeg() {

	_gridLocation.x -= 1;

	for (int fy = 0; fy < 3; fy++) {
		for (int fz = 0; fz < 3; fz++) {

			delete(_volumes[2][fy][fz]); //Aliberam memoria

			_volumes[2][fy][fz] = _volumes[1][fy][fz];
			_volumes[1][fy][fz] = _volumes[0][fy][fz];
			_volumes[0][fy][fz] = createNewVolume(_gridLocation.x, fy + _gridLocation.y, fz + _gridLocation.z);
		}
	}
	cout << _gridLocation.x << "\n";

}

//Movement Y
void  ProceduralGenerator::moveVolumeYPoz() {

	_gridLocation.y += 1;

	for (int fx = 0; fx < 3; fx++) {
		for (int fz = 0; fz < 3; fz++) {

			delete(_volumes[fx][0][fz]); //Aliberam memoria
			_volumes[fx][0][fz] = _volumes[fx][1][fz];
			_volumes[fx][1][fz] = _volumes[fx][2][fz];
			_volumes[fx][2][fz] = createNewVolume(fx + _gridLocation.x , _gridLocation.y + 2, fz + _gridLocation.z);
		}
	}
	cout << "\n";

}

void  ProceduralGenerator::moveVolumeYNeg() {

	_gridLocation.y -= 1;

	for (int fx = 0; fx < 3; fx++) {
		for (int fz = 0; fz < 3; fz++) {

			delete(_volumes[fx][2][fz]); //Aliberam memoria

			_volumes[fx][2][fz] = _volumes[fx][1][fz];
			_volumes[fx][1][fz] = _volumes[fx][0][fz];
			 _volumes[fx][0][fz] = createNewVolume(fx + _gridLocation.x, _gridLocation.y, fz + _gridLocation.z);
		}
	}
	cout << "\n";

}

//Movement Z
void  ProceduralGenerator::moveVolumeZPoz() {

	_gridLocation.z += 1;

	for (int fx = 0; fx < 3; fx++) {
		for (int fy = 0; fy < 3; fy++) {

			delete(_volumes[fx][fy][0]); //Aliberam memoria
			_volumes[fx][fy][0] = _volumes[fx][fy][1];
			_volumes[fx][fy][1] = _volumes[fx][fy][2];
			_volumes[fx][fy][2] = createNewVolume(fx + _gridLocation.x, fy + _gridLocation.y, _gridLocation.z + 2);
		}
	}
	cout << "\n";

}

void  ProceduralGenerator::moveVolumeZNeg() {

	_gridLocation.z -= 1;

	for (int fx = 0; fx < 3; fx++) {
		for (int fy = 0; fy < 3; fy++) {

			delete(_volumes[fx][fy][2]); //Aliberam memoria

			_volumes[fx][fy][2] = _volumes[fx][fy][1];
			_volumes[fx][fy][1] = _volumes[fx][fy][0];
			_volumes[fx][fy][0] = createNewVolume(fx + _gridLocation.x, fy + _gridLocation.y, _gridLocation.z);
		}
	}
	cout << "\n";

}

//Utility

void ProceduralGenerator::generateAllVolumes() {

	for (int fx = 0; fx < 3; fx++) {
		for (int fy = 0; fy < 3; fy++) {
			for (int fz = 0; fz < 3; fz++) {
				_volumes[fx][fy][fz] = createNewVolume(
					fx + _gridLocation.x,
					fy + _gridLocation.y,
					fz + _gridLocation.z
				);
			}
		}
	}
}

//Imputs are the offsets for perlin noise generation in qudrant cordinates
Volume* ProceduralGenerator::createNewVolume(int offsetX, int offsetY, int offsetZ) {


	//cout << " sampled [" << offsetX << " " << offsetY << " " << offsetZ << "]\n";
	offsetX += 500;
	offsetY += 500;
	offsetZ += 500;

	Volume* newVolume = new Volume(_sizeX, _sizeY, _sizeZ);

	for (int fx = 0; fx < _sizeX; fx++) {
		for (int fy = 0; fy < _sizeY; fy++) {
			for (int fz = 0; fz < _sizeZ; fz++) {

				double xSampleLoc = (fx + offsetX * _sizeCubesX) / _granularity;
				double ySampleLoc = (fy + offsetY * _sizeCubesY) / _granularity;
				double zSampleLoc = (fz + offsetZ * _sizeCubesZ) / _granularity;

				float noiseValue = _noiseGenerator.noise(
					xSampleLoc, ySampleLoc, zSampleLoc
				);

				newVolume->setElement(fx, fy, fz, noiseValue);
			}
		}
	}

	newVolume->computeCubes(_threshhold);
	return newVolume;
}

unique_ptr<vector<Triangle>> ProceduralGenerator::convertVolumeToGeometry(int x, int y, int z) {

	vector<Triangle>* trianglesToDraw = new vector<Triangle>;

	for (int f1 = 0; f1 < _sizeCubesX; f1++) {
		for (int f2 = 0; f2 < _sizeCubesY; f2++) {
			for (int f3 = 0; f3 < _sizeCubesZ; f3++) {

				int volumeIndex = _volumes[x][y][z]->getCube(f1, f2, f3).getIndex();
				for (Triangle trig : cubeConfiguration[volumeIndex].triangles)
				{

					float finalColor = ((float)(f3 + f1 + f2) / (5 * _sizeCubesX));
				//	float finalColorG = ((float)f2 / _sizeY);
				//	float finalColorB = ((float)f1 / _sizeZ);

					int valueToTranslateX = f1 + x * _sizeCubesX + _drawingOffset.x;
					int valueToTranslateY = f2 + y * _sizeCubesY + _drawingOffset.y;
					int valueToTranslateZ = f3 + z * _sizeCubesZ + _drawingOffset.z;

					trig.translate(glm::vec3(
						valueToTranslateX,
						valueToTranslateY,
						valueToTranslateZ
					));

					trig.color(glm::vec3(finalColor, finalColor, finalColor));
					trianglesToDraw->push_back(trig);
				}
			}
		}
	}
	return   unique_ptr<vector<Triangle>>(trianglesToDraw);
}

void ProceduralGenerator::changeSeed() {
	srand(time(NULL));
	_noiseGenerator = PerlinNoise((float)rand());
}

//Here we generatea all the variations of geometry that will be used to create the finam mesh
void ProceduralGenerator::generateCubeConfiguration() {

	for (int f1 = 0; f1 < 256; f1++) {
		//pentru fiecare tringhi
		cubeConfiguration[f1].triangles.clear();

		for (int f4 = 0; Tables::triTable[f1][f4] != -1; f4 += 3) {

			int vertex1Index = Tables::triTable[f1][f4];
			int vertex2Index = Tables::triTable[f1][f4 + 1];
			int vertex3Index = Tables::triTable[f1][f4 + 2];

			int a0 = Tables::cornerIndexAFromEdge[vertex1Index];
			int b0 = Tables::cornerIndexBFromEdge[vertex1Index];

			int a1 = Tables::cornerIndexAFromEdge[vertex2Index];
			int b1 = Tables::cornerIndexBFromEdge[vertex2Index];

			int a2 = Tables::cornerIndexAFromEdge[vertex3Index];
			int b2 = Tables::cornerIndexBFromEdge[vertex3Index];

			Triangle trig;
			trig.vertex1 = avarageVec3(corners[a0], corners[b0]);
			trig.vertex2 = avarageVec3(corners[a1], corners[b1]);
			trig.vertex3 = avarageVec3(corners[a2], corners[b2]);

			//Colors
			trig.color1 = glm::vec3(20, 20, 20);
			trig.color2 = glm::vec3(20, 20, 20);
			trig.color3 = glm::vec3(20, 20, 20);

			// Write to the file
			trig.normal1 = glm::cross(trig.vertex1 - trig.vertex2, trig.vertex1 - trig.vertex3);

			trig.normal2 = trig.normal1;
			trig.normal3 = trig.normal1;

			//We write what we need in our little cache
			cubeConfiguration[f1].triangles.push_back(trig);
		}
	}
}

//Auxiliary fuction
glm::vec3 avarageVec3(glm::vec3 polygon1, glm::vec3 polygon2) {
	glm::vec3 tmp = polygon1 + polygon2;

	tmp.x /= 2;
	tmp.y /= 2;
	tmp.z /= 2;

	return tmp;
}
