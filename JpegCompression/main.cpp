#include <cstdio>
#include <cstdlib>

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <sstream>
#include "jpeg_encoder.h"

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>


struct rgb {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};
typedef struct rgb rgb_t;

struct PPMimage {
	size_t w, h;
	rgb_t* pixel;
};
typedef struct PPMimage ppm_t;

int readPPMImage(const char* const file, size_t* width, size_t* height, rgb_t** buffer);

int readPPMImage(const char* const file, size_t* width, size_t* height, rgb_t** buffer)
{
	char line[0x80];
	char* tok;
	int ret;

	ret = 0;
	FILE* fp;
	ret = fopen_s(&fp, file, "rb");
	if (fp == NULL) {
		//fprintf(stderr, "Could not open file");
		return 0x1;
	}

	if (fgets(line, 0x80, fp) == NULL) {
		//fprintf(stderr, "Could not get content from file");
		ret = 0x2;
		goto end;
	}

	if (strcmp(line, "P6\n")) {
		//fprintf(stderr, "Illegal file format");
		ret = 0x3;
		goto end;
	}
	while (fgets(line, 0x80, fp)) {
		if (line[0] == '#')
			continue;
		else {
			char* next_tok;
			tok = strtok_s(line, " ", &next_tok);
			*width = atoi(tok);
			tok = strtok_s(NULL, " ", &next_tok);
			*height = atoi(tok);
			(void)fgets(line, 0x80, fp);
			break;
		}
	}

#ifdef __cplusplus
	* buffer = (rgb_t*)malloc(*width * *height * sizeof(rgb_t));
#else
	* buffer = malloc(*width * *height * sizeof(rgb_t));
#endif
	if (*buffer == NULL) {
		//fprintf(stderr, "Memory Allocation failed");
		ret = 0x4;
		goto end;
	}

	(void)fread(*buffer, sizeof(rgb_t), *width * *height, fp);

end:
	fclose(fp);
	return ret;
}

//////////////////////////////////////////////////////////////////////////////
// Main function
//////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv) {
	ppm_t image;

	//std::string image_path = cv::samples::findFile("sample3.jpg");
	//cv::Mat img = cv::imread(image_path, cv::IMREAD_COLOR);

	unsigned char quality = 100;
	const char* file = "sample.ppm";
	/* Create the encoder */
	jpeg::JPEGEncoder encoder(CL_DEVICE_TYPE_ALL, quality);

	/* Read input image */
	if (readPPMImage(file, &image.w, &image.h, &image.pixel))
	{
		fprintf(stderr, "Error Reading input file\naborting...\n");
		return 0x2;
	}

	const char* outFile = "sample4.jpg";
	
	/* Encode the image */
	encoder.encode_image((unsigned char*)image.pixel, image.w, image.h, outFile);
	//encoder.encode_image((unsigned char*)img.data, img.cols, img.rows, outFile);

	/* Free image memory */
	free(image.pixel);
	//free(img.data);

	return 0;
}