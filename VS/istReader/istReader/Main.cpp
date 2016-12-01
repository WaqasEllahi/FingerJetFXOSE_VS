#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include  <cstdint>
#include <vector>
using namespace std;

typedef enum {
	UNKNOWN_MINUTIA_TYPE = -1,
	ENDING_MINUTIA_TYPE = 1,
	BIFURCATION_MINUTIA_TYPE = 3

} MINUTIA_TYPE_T;

class Minutia {
public:
	int x;
	int y;
	int angle;
	MINUTIA_TYPE_T typ;
	int quality;
};

vector<Minutia> readIst(const string & fileName)
{
	FILE *in = fopen(fileName.c_str(), "rb");

	unsigned char header1[24];
	if (fread(header1, sizeof(unsigned char), 24, in) != 24) {
		cout << "file reading error" << endl;
	}

	int sizeX = (int)header1[15] + 0x100 * (int)header1[14];
	int sizeY = (int)header1[17] + 0x100 * (int)header1[16];
	int resX = (int)header1[19] + 0x100 * (int)header1[18];
	int resY = (int)header1[21] + 0x100 * (int)header1[20];

	unsigned char header[4];

	if (fread(header, sizeof(unsigned char), 4, in) != 4)
		cout << "file reading error" << endl;

	int impressionType = (header[1] >> 4) & 0xF;
	int numMinutiae = header[3];
	int fingerQuality = header[2];

	vector<Minutia> minVector;

	unsigned char minutiaData[6];
	for (int i = 0; i < numMinutiae; i++) {

		Minutia min;

		if (fread(minutiaData, sizeof(unsigned char), 6, in) != 6) {
			cout << "file reading error" << endl;
		}

		switch ((minutiaData[0] >> 6) & 0x3) {
		case 1:
			min.typ = ENDING_MINUTIA_TYPE;
			break;
		case 2:
			min.typ = BIFURCATION_MINUTIA_TYPE;
			break;
		default:
			min.typ = UNKNOWN_MINUTIA_TYPE;
			break;
		}

		uint16_t x, y;
		x = 0x100 * (minutiaData[0] & ((1 << 6) - 1)) + minutiaData[1];
		y = 0x100 * (minutiaData[2] & ((1 << 6) - 1)) + minutiaData[3];

		double angle = ((M_PI + M_PI) * ((double)minutiaData[4] / 256.0)) + M_PI;

		uint8_t quality = minutiaData[5];

		min.x = x;
		min.y = y;
		min.angle = (angle* 180.0 / M_PI);
		min.quality = quality;

		minVector.push_back(min);
		
		//cout << "X: " << x << " Y: " << y << endl;
	}

	fclose(in);

	return minVector;
}

void writeXYT(const string & fileName, vector<Minutia> minVector)
{
	FILE *fptr;
	fptr = fopen(fileName.c_str(), "w");

	if (fptr == NULL)
	{
		printf("Error!");
		exit(1);
	}

	/*for (size_t i = 0; i < md.size(); i++) {
		fprintf(fptr, "%d %d %d %d\n", md.minutia[i].position.x,
			md.minutia[i].position.y, md.minutia[i].theta, md.minutia[i].type);
	}*/

	vector<Minutia>::iterator min = minVector.begin();
	while (min != minVector.end()) {
		//cout << "X: " << (*min).x << " Y: " << (*min).y << endl;
		fprintf(fptr, "%d %d %d %d\n", (*min).x, (*min).y, (*min).angle, (*min).typ);
		min++;
	}

	fclose(fptr);
}

void main()
{
	char xytFilePath[200], xytFileName[200];
	char istFilePath[200], istFileName[200];
	
	strcpy(istFilePath, "D:/Kashif/Development/fingerjetfx/sample/ist/FVCDB20042IST/");
	strcpy(xytFilePath, "D:/Kashif/Development/fingerjetfx/sample/xyt/FVCDB20042XYT/");
	for (int subId = 1; subId <= 100; subId++){
		for (int fingerId = 1; fingerId <= 8; fingerId++){
			sprintf(istFileName, "%s%d_%d.ist", istFilePath, subId, fingerId);
			sprintf(xytFileName, "%s%d_%d.xyt", xytFilePath, subId, fingerId);
			printf("file process %s\n", istFileName);
			//vector<Minutia> minVector = readIst("C:/Users/TNDUser/Desktop/analysis/1_1.ist");
			//writeXYT("C:/Users/TNDUser/Desktop/analysis/fingerJet/1_1_ist.xyt", minVector);
			vector<Minutia> minVector = readIst(istFileName);
			writeXYT(xytFileName, minVector);
		}
	}


}