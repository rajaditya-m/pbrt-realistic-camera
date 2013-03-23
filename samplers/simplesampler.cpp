//
//  simplesampler.cpp
//  pbrt
//
//  Created by Rajaditya Mukherjee on 3/1/13.
//
//

#include "montecarlo.h"
#include "simplesampler.h"


void SimpleStratifiedSampler::reset( RNG& rng )
{
	xPos = xPixelStart;
	yPos = yPixelStart;
    
	// Generate stratified camera samples for (_xPos_,_yPos_)
	StratifiedSample2D(imageSamples,
                       xPixelSamples, yPixelSamples, rng,
                       jitterSensorSamples);
	StratifiedSample2D(lensSamples,
                       xPixelSamples, yPixelSamples, rng,
                       jitterNonSensorSamples);
	StratifiedSample1D(timeSamples, xPixelSamples*yPixelSamples, rng,
                       jitterNonSensorSamples);
	// Shift stratified image samples to pixel coordinates
	for (int o = 0; o < 2 * xPixelSamples * yPixelSamples; o += 2) {
		imageSamples[o]   += xPos;
		imageSamples[o+1] += yPos;
	}
	// Decorrelate sample dimensions
	Shuffle(lensSamples, xPixelSamples*yPixelSamples, 2, rng);
	Shuffle(timeSamples, xPixelSamples*yPixelSamples, 1, rng);
	samplePos = 0;
    
}

// This is a sampler for use when producing an image of size pixelsX by pixelsY
// a total of samplesPerDim*samplerPerDim samples will be taken per pixel.
// See PBRT for a description of what other samples (other than spatial samples on
// the film) that Samplers generate.
// jitterSensor -- whether or not to jitter spatial samples on the sensor
// jitterNonSensor -- whether or not to jitter all other samples
SimpleStratifiedSampler::SimpleStratifiedSampler(int pixelsX, int pixelsY,
                                                 int samplesPerDim, bool jitterSensor, bool jitterNonSensor, RNG& rng)
{
    
	jitterSensorSamples = jitterSensor;
	jitterNonSensorSamples = jitterNonSensor;
	xPixelStart = 0;
	xPixelEnd = pixelsX;
	yPixelStart = 0;
	yPixelEnd = pixelsY;
	xPixelSamples = samplesPerDim;
	yPixelSamples = samplesPerDim;
    
	// Allocate storage for a pixel's worth of stratified samples
	imageSamples = (float *)AllocAligned(5 * xPixelSamples *
                                         yPixelSamples * sizeof(float));
	lensSamples =
    imageSamples + 2 * xPixelSamples * yPixelSamples;
	timeSamples =
    lensSamples + 2 * xPixelSamples * yPixelSamples;
    
	reset( rng );
    
}
bool SimpleStratifiedSampler::GetNextSample(Sample *sample, RNG& rng ) {
	// Compute new set of samples if needed for next pixel
    
	if (samplePos == xPixelSamples * yPixelSamples) {
		// Advance to next pixel for stratified sampling
		if (++xPos == xPixelEnd) {
			xPos = xPixelStart;
			++yPos;
		}
		if (yPos == yPixelEnd)
			return false;
        
		// Generate stratified camera samples for (_xPos_,_yPos_)
		StratifiedSample2D(imageSamples,
                           xPixelSamples, yPixelSamples, rng,
                           jitterSensorSamples);
		StratifiedSample2D(lensSamples,
                           xPixelSamples, yPixelSamples, rng,
                           jitterNonSensorSamples);
		StratifiedSample1D(timeSamples, xPixelSamples*yPixelSamples, rng,
                           jitterNonSensorSamples);
		// Shift stratified image samples to pixel coordinates
		for (int o = 0; o < 2 * xPixelSamples * yPixelSamples; o += 2) {
			imageSamples[o]   += xPos;
			imageSamples[o+1] += yPos;
		}
		// Decorrelate sample dimensions
		Shuffle(lensSamples, xPixelSamples*yPixelSamples, 2, rng );
		Shuffle(timeSamples, xPixelSamples*yPixelSamples, 1, rng );
		samplePos = 0;
	}
	// Return next _SimpleStratifiedSampler_ sample point
	sample->imageX = imageSamples[2*samplePos];
	sample->imageY = imageSamples[2*samplePos+1];
	sample->lensU = lensSamples[2*samplePos];
	sample->lensV = lensSamples[2*samplePos+1];
	sample->time = timeSamples[samplePos];
	// Generate stratified samples for integrators
	for (u_int i = 0; i < sample->n1D.size(); ++i)
		LatinHypercube(sample->oneD[i], sample->n1D[i], 1, rng);
	for (u_int i = 0; i < sample->n2D.size(); ++i)
		LatinHypercube(sample->twoD[i], sample->n2D[i], 2, rng);
	++samplePos;
	return true;
}
