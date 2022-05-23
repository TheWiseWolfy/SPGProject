
#include "ProceduralGenerator.h"
#include "LookUpTables.h"
#include "CubeConfiguration.h"

using namespace std;

//Private fuctions 
glm::vec3 avarageVec3(glm::vec3 polygon1, glm::vec3 polygon2);
std::vector<CubeConfiguration> cubeConfiguration(256);

//Generates 9 chunks around the player, when the player moves , 9 new chunks are generated in the right direction

vector<Triangle> ProceduralGenerator::GenerateAroundPlayer(glm::vec3 playerPoz) {

	_playerQuadrant.x = playerPoz.x / _sizeCubesX;
	_playerQuadrant.y = playerPoz.y / _sizeCubesY;
	_playerQuadrant.z = playerPoz.z / _sizeCubesZ;

	for (int f1 = 0; f1 < 3; f1++) {
		for (int f2 = 0; f2 < 3; f2++) {
			for (int f3 = 0; f3 < 3; f3++) {

				cout << _playerQuadrant.x << " " << _playerQuadrant.y << " " << _playerQuadrant.z << "\n";
			//	cout << _drawingOffset.x << "\n";

				if (_playerQuadrant.x >0) {
					if (_playerQuadrant.x > _oldPlayerQuadrant.x) {
						 moveVolumeXPoz( _playerQuadrant.x) ;
					}
					else if (_playerQuadrant.x < _oldPlayerQuadrant.x) {
						moveVolumeXNeg(_playerQuadrant.x);
					}
				}
				else if (_playerQuadrant.x < 0)
				{
					if (_playerQuadrant.x < _oldPlayerQuadrant.x) {
						moveVolumeXPoz(_playerQuadrant.x);
					}
					else if (_playerQuadrant.x > _oldPlayerQuadrant.x) {
						moveVolumeXNeg(_playerQuadrant.x);
					}
				}
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
				vector<Triangle>& volumeGeometry = convertVolumeToGeometry(fx, fy, fz);
				trianglesToDraw.insert(trianglesToDraw.end(), volumeGeometry.begin(), volumeGeometry.end());
			}
		}
	}
	return trianglesToDraw;
}

void  ProceduralGenerator::moveVolumeXPoz(int x) {
	for (int fy = 0; fy < 3; fy++) {
		for (int fz = 0; fz < 3; fz++) {

			delete(_volumes[0][fy][fz]); //Aliberam memoria
			_volumes[0][fy][fz] = _volumes[1][fy][fz];
			_volumes[1][fy][fz] = _volumes[2][fy][fz];
			_volumes[2][fy][fz] = createNewVolume(x, fy, fz);
			_drawingOffset.x = x * _sizeCubesX;
		}
	}
}

void  ProceduralGenerator::moveVolumeXNeg(int x) {

	for (int fy = 0; fy < 3; fy++) {
		for (int fz = 0; fz < 3; fz++) {

			delete(_volumes[2][fy][fz]); //Aliberam memoria

			_volumes[2][fy][fz] = _volumes[1][fy][fz];
			_volumes[1][fy][fz] = _volumes[0][fy][fz];
			_volumes[0][fy][fz] = createNewVolume(x, fy, fz);
			_drawingOffset.x = x * _sizeCubesX;
		}
	}
}


void ProceduralGenerator::generateAllVolumes() {

	for (int fx = 0; fx < 3; fx++) {
		for (int fy = 0; fy < 3; fy++) {
			for (int fz = 0; fz < 3; fz++) {
				_volumes[fx][fy][fz] = createNewVolume(
					 fx,
					 fy,
					 fz
				);
			}
		}
	}
}

//Imputs are the offsets for perlin noise generation in qudrant cordinates
Volume* ProceduralGenerator::createNewVolume(int offsetX, int offsetY, int offsetZ) {

	Volume* newVolume = new Volume(_sizeX, _sizeY, _sizeZ);

	offsetX += 100;
	offsetY += 100;
	offsetZ += 100;

	for (int fx = 0; fx < _sizeX; fx++) {
		for (int fy = 0; fy < _sizeY; fy++) {
			for (int fz =0; fz < _sizeZ; fz++) {

				double xSampleLoc = (fx  + offsetX * _sizeCubesX) / _granularity ;
				double ySampleLoc = (fy + offsetY * _sizeCubesY) / _granularity;
				double zSampleLoc = (fz + offsetZ * _sizeCubesZ) / _granularity;

				float noiseValue = _noiseGenerator.noise( 
					xSampleLoc, ySampleLoc, zSampleLoc	
				);

				newVolume->setElement(fx, fy, fz, noiseValue);
			}
		}
	}

	newVolume->computeCubes();
	return newVolume;
}

vector<Triangle>& ProceduralGenerator::convertVolumeToGeometry(int x,int y, int z) {

	vector<Triangle>* trianglesToDraw = new vector<Triangle>;

	for (int f1 = 0; f1 < _sizeCubesX; f1++) {
		for (int f2 = 0; f2 < _sizeCubesY; f2++) {
			for (int f3 = 0; f3 < _sizeCubesZ; f3++) {

				int volumeIndex = _volumes[x][y][z]->getCube(f1, f2, f3).getIndex();
				for(Triangle trig : cubeConfiguration[volumeIndex].triangles )
				{

					float finalColorR = ((float)f3 / _sizeX);
					float finalColorG = ((float)f2 / _sizeY);
					float finalColorB = ((float)f1 / _sizeZ); 

					int valueToTranslateX = f1 + x * _sizeCubesX + _drawingOffset.x;
					int valueToTranslateY = f2 + y * _sizeCubesY + _drawingOffset.y;
					int valueToTranslateZ = f3 + z * _sizeCubesZ + _drawingOffset.z;

					trig.translate(glm::vec3(
						valueToTranslateX,
						valueToTranslateY,
						valueToTranslateZ
					));

					trig.color(glm::vec3(finalColorR, finalColorG, finalColorB));
					trianglesToDraw->push_back(trig);
				}
			}
		}
	}
	return *trianglesToDraw;
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
			trig.color1 = glm::vec3(20,20,20);
			trig.color2 = glm::vec3(20,20,20);
			trig.color3 = glm::vec3(20,20,20);

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
