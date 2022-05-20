#include "ProceduralGenerator.h"
#include "LookUpTables.h"
#include "Triangle.h"
#include "Volume.h"


using namespace std;

//Private fuctions 
glm::vec3 avarageVec3(glm::vec3 polygon1, glm::vec3 polygon2);

void changeSeed();

vector<Triangle> ProceduralGenerator::generateGeometry() {

	_volume.clear();

	for (int f1 = 0; f1 < _sizeX; f1++) {
		for (int f2 = 0; f2 < _sizeY; f2++) {
			for (int f3 = 0; f3 < _sizeZ; f3++) {

				float maxHeight = _noiseGenerator.noise(
					f1 / (float)_sizeX * _granularity, 
					f2 / (float)_sizeY * _granularity,
					f3 / (float)_sizeZ * _granularity);
				_volume.setElement(f1, f3, f2, maxHeight);
			}


			//float noise = noiseGenerator.noise(f1 / (float)sizeV * granularity + offset, f2 / (float)sizeV * granularity + offset ,0.7 );
			//int actualHeight = noise * sizeV ;

			//if (actualHeight > sizeV) {
			//	actualHeight = sizeV ;
			//}

			//if (actualHeight < 0) {
			//	actualHeight = 0;
			//}

			//for (int f3 = 0; f3 < actualHeight; f3++) {

			//	_volume.setElement(f1, f3, f2, 1);
			//}
		}
	}

	return generateTerrainGeometry();
}



vector<Triangle> ProceduralGenerator::generateTerrainGeometry() {
	_volume.computeCubes();

	vector<Triangle> trianglesToDraw;

	for (int f1 = 0; f1 < _sizeX - 1; f1++) {

		for (int f2 = 0; f2 < _sizeY - 1; f2++) {

			for (int f3 = 0; f3 < _sizeZ - 1; f3++) {

				int volumeIndex = _volume.getCube(f1, f2, f3).getIndex();

				//pentru fiecare tringhi
				for (int f4 = 0; Tables::triTable[volumeIndex][f4] != -1; f4 += 3) {

					int vertex1Index = Tables::triTable[volumeIndex][f4];
					int vertex2Index = Tables::triTable[volumeIndex][f4 + 1];
					int vertex3Index = Tables::triTable[volumeIndex][f4 + 2];

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

					float finalColorR = ((float)f3 / _sizeX);
					float finalColorG = ((float)f2 / _sizeY);
					float finalColorB = ((float)f1 / _sizeZ);


					trig.color1.r = finalColorR;
					trig.color1.g = finalColorG;
					trig.color1.b = finalColorB;

					trig.color2.r = finalColorR;
					trig.color2.g = finalColorG;
					trig.color2.b = finalColorB;

					trig.color3.r = finalColorR;
					trig.color3.g = finalColorG;
					trig.color3.b = finalColorB;

					trig.normal1.x = trig.vertex1.y * trig.vertex2.z - trig.vertex1.z * trig.vertex2.y;
					trig.normal1.y = trig.vertex1.z * trig.vertex2.x - trig.vertex1.x * trig.vertex2.z;
					trig.normal1.z = trig.vertex1.x * trig.vertex2.y - trig.vertex1.y * trig.vertex2.x;

					trig.mofify(f1, f2, f3);
					trianglesToDraw.push_back(trig);
				}
			}
		}
	}

	return trianglesToDraw;
}

void ProceduralGenerator::changeSeed() {
	srand(time(NULL));
	_noiseGenerator = PerlinNoise((float)rand());
}


glm::vec3 avarageVec3(glm::vec3 polygon1, glm::vec3 polygon2) {
	glm::vec3 tmp = polygon1 + polygon2;

	tmp.x /= 2;
	tmp.y /= 2;
	tmp.z /= 2;

	return tmp;
}
