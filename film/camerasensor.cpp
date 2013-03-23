//
//  camerasensor.cpp
//  pbrt
//
//  Created by Rajaditya Mukherjee on 3/1/13.
//
//

#include "pbrt.h"
#include "imageio.h"
#include "camerasensor.h"

// CameraSensor Method Definitions
CameraSensor::CameraSensor(int xres, int yres) : pixels(xres, yres)
{
	xPixelCount = xres;
	yPixelCount = yres;
    
	// Allocate film image storage
	rgb = new float[3 * xPixelCount * yPixelCount];
	alpha = new float[xPixelCount * yPixelCount];
}


// accumulates sample into one pixel of the sensor using nearest neighbor
// filtering
void CameraSensor::AddSample(int pixelx, int pixely, const Spectrum &L, const Spectrum& alpha) {
    
	float weight = 1.0f;
    
	Pixel &pixel = pixels(pixelx, pixely);
	//pixel.L.AddWeighted(weight, L);
	pixel.L += L;
	pixel.alpha += alpha;
	pixel.weightSum += weight;
}


// clears the image
void CameraSensor::ResetImage() {
    
	for (int i=0; i<xPixelCount; i++)
		for (int j=0; j<yPixelCount; j++) {
			pixels(i,j).Reset();
		}
}


// Return an array of floats representing the RGB image. Image pixels
// are returned in row major order. RGB values for a single pixel are
// consecutive elements in this array.
float* CameraSensor::ComputeImageRGB() {
    
	int nPix = xPixelCount * yPixelCount;
	int offset = 0;
	for (int y = 0; y < yPixelCount; ++y) {
		for (int x = 0; x < xPixelCount; ++x) {
			// Convert pixel spectral radiance to RGB
			float xyz[3];
			float a[3];
			pixels(x, y).L.ToXYZ(xyz);
			const float
            rWeight[3] = { 3.240479f, -1.537150f, -0.498535f };
			const float
            gWeight[3] = {-0.969256f,  1.875991f,  0.041556f };
			const float
            bWeight[3] = { 0.055648f, -0.204043f,  1.057311f };
			rgb[3*offset  ] = rWeight[0]*xyz[0] +
            rWeight[1]*xyz[1] +
            rWeight[2]*xyz[2];
			rgb[3*offset+1] = gWeight[0]*xyz[0] +
            gWeight[1]*xyz[1] +
            gWeight[2]*xyz[2];
			rgb[3*offset+2] = bWeight[0]*xyz[0] +
            bWeight[1]*xyz[1] +
            bWeight[2]*xyz[2];
			pixels(x, y).alpha.ToRGB( a );
			alpha[offset] = a[0];
            
			// Normalize pixel with weight sum
			float weightSum = pixels(x, y).weightSum;
			if (weightSum != 0.f) {
				float invWt = 1.f / weightSum;
				rgb[3*offset  ] =
                Clamp(rgb[3*offset  ] * invWt, 0.f, INFINITY);
				rgb[3*offset+1] =
                Clamp(rgb[3*offset+1] * invWt, 0.f, INFINITY);
				rgb[3*offset+2] =
                Clamp(rgb[3*offset+2] * invWt, 0.f, INFINITY);
				alpha[offset] = Clamp(alpha[offset] * invWt, 0.f, 1.f);
			}
            
			// Compute premultiplied alpha color
			rgb[3*offset  ] *= alpha[offset];
			rgb[3*offset+1] *= alpha[offset];
			rgb[3*offset+2] *= alpha[offset];
			
			++offset;
		}
	}
    
    
	// UA: Uncomment the following lines when debugging to write
	// the current state of the image to a unique file on disk.
	// Potentially very useful.
    
	/*
     
     static int afCount = 0;
     char filename[30];
     
     // Write RGBA image to disk
     sprintf(filename, "afzone%04d.exr", afCount);
     printf("Writing image %s\n", filename);
     WriteImage(filename, rgb, alpha,
     xPixelCount, yPixelCount,
     xPixelCount, yPixelCount,
     0, 0);
     
     afCount++;
     
     */
    
    
	return rgb;
}
