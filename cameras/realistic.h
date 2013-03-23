//
//  realistic.h
//  pbrt
//
//  Created by Rajaditya Mukherjee on 3/1/13.
//
//

#ifndef __pbrt__realistic__
#define __pbrt__realistic__

#include <iostream>
#include <vector>

struct lensData
{
    float radius;
    float aperture;
    float refractiveIndex;
    float axisPos;
    float centerInCameraAxis;
    float distToFilm;
};

class RealisticCamera : public Camera {
public:
    RealisticCamera(const AnimatedTransform &cam2world,
                    float hither, float yon, float sopen,
                    float sclose, float filmdistance, float aperture_diameter,
                    const string &specfile,
                    const string &autofocusfile,
                    float filmdiag,
                    Film *f);
    ~RealisticCamera();
    float GenerateRay(const CameraSample &sample, Ray *) const;
    Point RasterToCamera(Point &p) const;
protected:
    float clipHither;
    float clipYon;
    float filmDiagonal;
    float apertureDiameter;
    float filmDistance;
    float xCamFilmRes;
    float yCamFilmRes;
    int numLensInterface;
    float filmPlaneZ;
    Film* film;
    float backLensArea;
    
private:
    std::vector<lensData> lensInformation;
    bool  autofocus;

};

RealisticCamera* CreateRealisticCamera(const ParamSet &params,const AnimatedTransform &cam2world, Film *film);

#endif /* defined(__pbrt__realistic__) */
