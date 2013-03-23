//
//  simplesampler.h
//  pbrt
//
//  Created by Rajaditya Mukherjee on 3/1/13.
//
//

#ifndef __pbrt__simplesampler__
#define __pbrt__simplesampler__

#include <iostream>
#include "memory.h"
#include "sampler.h"



class SimpleStratifiedSampler {
    
public:
    
	// This is a sampler for use when producing an image of size pixelsX by pixelsY
	// a total of samplesPerDim*samplerPerDim samples will be taken per pixel.
	// See PBRT for a description of what other samples (other than spatial samples on
	// the film) that Samplers generate.
	// jitterSensor -- whether or not to jitter spatial samples on the sensor
	// jitterNonSensor -- whether or not to jitter all other samples
	SimpleStratifiedSampler(int pixelsX, int pixelsY,
							int samplesPerDim, bool jitterSensor, bool jitterNonSensor, RNG& rng);
    
	~SimpleStratifiedSampler() {
		FreeAligned(imageSamples);
	}
    
	bool GetNextSample(Sample *sample, RNG& rng);
	void reset( RNG& rng);
    
private:
    
	int xPixelStart, yPixelStart;
	int xPixelEnd, yPixelEnd;
	int xPixelSamples, yPixelSamples;
	bool jitterSensorSamples;
	bool jitterNonSensorSamples;
	int xPos, yPos;
	float *imageSamples, *lensSamples, *timeSamples;
	int samplePos;
	
};


#endif /* defined(__pbrt__simplesampler__) */
