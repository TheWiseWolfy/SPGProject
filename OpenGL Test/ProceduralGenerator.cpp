
#include "ProceduralGenerator.h"
#include "LookUpTables.h"
#include "CubeConfiguration.h"

using namespace std;

//Private fuctions 
glm::vec3 avarageVec3(glm::vec3 polygon1, glm::vec3 polygon2);
std::vector<CubeConfiguration> cubeConfiguration(256);

vector<Triangle> ProceduralGenerator::GenerateAroundPlayer(glm::vec3 playerPoz) {

	glm::ivec3 playerQuadrant;

	playerQuadrant.x = playerPoz.x / _sizeX;
	playerQuadrant.y = playerPoz.y / _sizeY;
	playerQuadrant.z = playerPoz.z / _sizeZ;

	for (int f1 = 0; f1 < 3; f1++) {
		for (int f2 = 0; f2 < 3; f2++) {
			for (int f3 = 0; f3 < 3; f3++) {
				

				glm::vec3 startOfChunk = glm::vec3(playerQuadrant.x * (int)_sizeX ,
													playerQuadrant.y * (int)_sizeY,
													playerQuadrant.z * (int)_sizeZ );


				glm::vec3 endOfChunk = glm::vec3(playerQuadrant.x * (int)_sizeX   ,
													playerQuadrant.y * (int)_sizeY,
													playerQuadrant.z * (int)_sizeZ );


	//			cout << playerQuadrant.x + f1 << " " << playerQuadrant.y + f2 << " " << playerQuadrant.z + f3 << "\n";

				generateVolume(startOfChunk, endOfChunk);
			}
		}
	}
	cout << "\n\n\n";


	return generateTerrainGeometry();
}

void ProceduralGenerator::generateVolume(glm::vec3 startPoint, glm::vec3 endPoint ) {

	for (int f1 = startPoint.x; f1 < endPoint.x; f1++) {
		for (int f2 = startPoint.y; f2 < endPoint.y; f2++) {
			for (int f3 = startPoint.z; f3 < endPoint.y; f3++) {

				float noiseValue = _noiseGenerator.noise(
					f1 / (float)_sizeX * _granularity, 
					f2 / (float)_sizeY * _granularity,
					f3 / (float)_sizeZ * _granularity);
				_volume.setElement(f1, f3, f2, noiseValue);
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

}

vector<Triangle> ProceduralGenerator::generateTerrainGeometry() {
	_volume.computeCubes();

	// Create and open a text file
	ofstream MyFile("filename.txt");

	vector<Triangle> trianglesToDraw;

	for (int f1 = 0; f1 < _sizeX - 1; f1++) {
		for (int f2 = 0; f2 < _sizeY - 1; f2++) {
			for (int f3 = 0; f3 < _sizeZ - 1; f3++) {

				int volumeIndex = _volume.getCube(f1, f2, f3).getIndex();
				for(Triangle trig : cubeConfiguration[volumeIndex].triangles )
				{

					float finalColorR = ((float)f3 / _sizeX);
					float finalColorG = ((float)f2 / _sizeY);
					float finalColorB = ((float)f1 / _sizeZ); 

					trig.move(glm::vec3(f1, f2, f3));
					trig.color(glm::vec3(finalColorR, finalColorG, finalColorB));
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
			trig.color1.r = 20;
			trig.color1.g = 20;
			trig.color1.b = 20;

			trig.color2.r = 20;
			trig.color2.g = 20;
			trig.color2.b = 20;

			trig.color3.r = 20;
			trig.color3.g = 20;
			trig.color3.b = 20;

			// Write to the file
			trig.normal1 = glm::cross(trig.vertex1 - trig.vertex2, trig.vertex1 - trig.vertex3);

			trig.normal2 = trig.normal1;
			trig.normal3 = trig.normal1;

			//We write what we need in our little cache
			cubeConfiguration[f1].triangles.push_back(trig);
			}
	}
}
