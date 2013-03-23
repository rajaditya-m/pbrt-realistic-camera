//
//  camerasensor.h
//  pbrt
//
//  Created by Rajaditya Mukherjee on 3/1/13.
//
//

#ifndef __pbrt__camerasensor__
#define __pbrt__camerasensor__

#include <iostream>

#include "pbrt.h"
#include "spectrum.h"
#include "memory.h"


/*
 The CameraSensor class is a very lightweight version of pbrt's existing Film class.
 Filters radiance samples onto sensor pixels via point sampling and allows the state
 of the sensor to be reset.
 */

class CameraSensor {
public:
    
	struct Pixel {
		Pixel() : L(0.f) {
			alpha = 0.f;
			weightSum = 0.f;
		}
        
		void Reset() {
			alpha = 0.0f;
			weightSum = 0.0f;
			L = Spectrum(0.0f);
		}
        
		Spectrum L;
		Spectrum alpha;
		float weightSum;
	};
    
    
	CameraSensor(int xres, int yres);
    
	~CameraSensor() {
		delete[] rgb;
		delete[] alpha;
	}
    
	void   AddSample(int pixelx, int pixely, const Spectrum &L, const Spectrum& alpha);
	float* ComputeImageRGB();
	void   ResetImage();
    
public:
	int xPixelCount, yPixelCount;
    
private:
	float *rgb;
	float *alpha;
    
	BlockedArray<Pixel> pixels;	
    
};

#endif /* defined(__pbrt__camerasensor__) */
