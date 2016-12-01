/*
FingerJetFX OSE VS : This is visual studio solution for FingerJetFX OSE
Converted by : Waqas Ellahi(waqas_ellahi@yahoo.com)
*/
/*
FingerJetFX OSE -- Fingerprint Feature Extractor, Open Source Edition

Copyright (c) 2011 by DigitalPersona, Inc. All rights reserved.

DigitalPersona, FingerJet, and FingerJetFX are registered trademarks
or trademarks of DigitalPersona, Inc. in the United States and other
countries.

FingerJetFX OSE is open source software that you may modify and/or
redistribute under the terms of the GNU Lesser General Public License
as published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version, provided that the
conditions specified in the COPYRIGHT.txt file provided with this
software are met.

For more information, please visit digitalpersona.com/fingerjetfx.

/*
BINARY: fjfxSample - Sample Code for Fingerprint Feature Extractor


ALGORITHM:      Alexander Ivanisov
Yi Chen
Salil Prabhakar
IMPLEMENTATION: Alexander Ivanisov
Jacob Kaminsky
Lixin Wei
DATE:           11/08/2011
*/

#include <stdio.h>
#include <stdlib.h>
#include <fjfx.h>
#include <iostream>

int main(int argc, char ** argv) {
	FILE *fp = 0;
	int height, width, gray;
	unsigned int size;
	void * image = 0;
	size_t n;
	int err;
	unsigned char tmpl[FJFX_FMD_BUFFER_SIZE] = { 0 };
	int subId, fingerId, dbSize =100, sampleSize = 8;
	
	char pgmFilePath[500];
	char istFilePath[500];
	char pgmFileName[500];
	char istFileName[500];

  
	std::cout << "Fingerprint Minutia Extraction\n";

	if (argc != 5 || argv[1] == NULL || argv[2] == NULL || argv[3] == NULL || argv[4] == NULL) {

		std::cout << "Invalid Arguments" << std::endl;
		std::cout << "Usage: <database size> <sample size> <input directory> <output directory>" << std::endl;
		system("PAUSE");
	
	}else{

		std::cout << "Database Size: " << argv[1] << ", Sample Size: " << argv[2] << std::endl;
		std::cout << "Input Directory: " << argv[3] << "\nOutput Directory: " << argv[4] << std::endl;

		strcpy(pgmFilePath, argv[3]);
		strcpy(istFilePath, argv[4]);

		for (subId = 1; subId <= dbSize; subId++)
		{
			for (fingerId = 1; fingerId <= sampleSize; fingerId++)
			{

				std::sprintf(pgmFileName, "%s%d_%d.pgm", pgmFilePath, subId, fingerId);
				std::sprintf(istFileName, "%s%d_%d.ist", istFilePath, subId, fingerId);

				std::cout << subId << "_" << fingerId << ".pgm" << " processed" << std::endl;


				fp = fopen(pgmFileName, "rb");
				if (fp == 0) {
					printf("Cannot open image file: %s\n", pgmFileName);
					return 9;
				}
				n = fscanf(fp, "P5%d%d%d", &width, &height, &gray);
				if (n != 3 ||
					gray > 256 || width > 0xffff || height > 0xffff ||
					gray <= 1 || width < 32 || height < 32) {
					printf("Image file %s is in unsupported format\n", pgmFileName);
					fclose(fp);
					return 10;
				}

				size = width * height;
				image = malloc(size);
				if (image == 0) {
					printf("Cannot allocate image buffer: image size is %dx%d", width, height);
					return 12;
				}

				n = fread(image, 1, size, fp);
				fclose(fp); fp = 0;
				if (n != size) {
					printf("Image file %s is too short\n", pgmFileName);
					free(image);
					return 11;
				}

				size = FJFX_FMD_BUFFER_SIZE;
				err = fjfx_create_fmd_from_raw(image, 500, height, width, FJFX_FMD_ISO_19794_2_2005, tmpl, &size);
				free(image); image = 0;
				if (err != FJFX_SUCCESS) {
					printf("Failed feature extraction\n");
					return err;
				}

				fp = fopen(istFileName, "wb");
				if (fp == 0) {
					printf("Cannot create output file: %s\n", istFileName);
					return 14;
				}
				n = fwrite(tmpl, 1, size, fp);
				fclose(fp);
				if (n != size) {
					printf("Cannot write output file of size %d\n", size);
					free(image);
					return 15;
				}
			}

		}
		system("PAUSE");
	}
}
