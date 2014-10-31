/** texture.cpp **/

#include "global.hpp"
#include <cmath>

//Creates a noisy texture
//channels can be only 3 or 4
unsigned char *create_texture1(const short width, const short height, const char channels) {
	if(channels != 3 && channels != 4) return 0;
	unsigned char *pixels = (unsigned char*)malloc(width * height * sizeof(unsigned char));
	unsigned char *pixels2 = (unsigned char*)malloc(width * height * channels * sizeof(unsigned char));
	for(int i=0;i<width*height;i++) {
		pixels[i] = 255 - (rand() % 100);
	}
	for(int i=0;i<width*height;i++) {
		float sum = 1;
		float amount = pixels[i];
		for(int j=0;j<15;j++) {
			for(int k=0;k<15;k++) {
				const float weight = 1.0f - float(j + k) / 15.0f;
				if(weight < 0) break;
				sum+= weight * 4;
				int location = 0;
				for(int a=0;a<4;a++) {
					switch(a) {
						case 0: location = i - j * height - k; break;
						case 1: location = i + j * height - k; break;
						case 2: location = i - j * height + k; break;
						case 3: location = i + j * height + k; break;
					}
					if(location < 0) location+= width * height;
					if(location >= width * height) location-= width * height;
					amount+= (float)pixels[location] * weight;
				}
			}
		}
		amount = round(amount / sum + rand() % 15 + 35);
		if(amount > 255) amount = 255;
		pixels2[i * channels] = (int)amount;
		pixels2[i * channels + 1] = pixels2[i * channels];
		pixels2[i * channels + 2] = pixels2[i * channels];
		if(channels == 4) pixels[i * channels + 3] = 0;
	}
	free(pixels);
	return pixels2;
}
