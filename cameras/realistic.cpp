//
//  realistic.cpp
//  pbrt
//
//  Created by Rajaditya Mukherjee on 3/1/13.
//
//
#include "pbrt.h"
#include "camera.h"
#include "paramset.h"
#include "film.h"
#include "scene.h"
#include "intersection.h"
#include "montecarlo.h"

#include "realistic.h"

#include "film/camerasensor.h"
#include "samplers/simplesampler.h"
#include "renderers/samplerrenderer.h"


#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <string>
#include <cmath>

#define SCALINGFAC 5

using namespace std;

bool bendBySnellsLaw(Ray r,Point p,lensData l,float n1,float n2,Ray *out);
bool testIntersectionWithLensElement( Ray r,lensData l ,float *t);

RealisticCamera::RealisticCamera(const AnimatedTransform &cam2world,
                                 float hither, float yon,
                                 float sopen, float sclose,
                                 float filmdistance, float aperture_diameter,
                                 const string &specfile,
								 const string &autofocusfile,
                                 float filmdiag,
								 Film *f)
: Camera(cam2world, sopen, sclose, f),
clipHither(hither), clipYon(yon)
{
    film = f;
    filmDistance=filmdistance;
    apertureDiameter=aperture_diameter;
    filmDiagonal=filmdiag;
    numLensInterface = 0;
    
    //Calculate effective camera film resolutions
    float aspectRatio = f->xResolution/f->yResolution;
    yCamFilmRes = sqrt((filmDiagonal*filmDiagonal)/(1+(aspectRatio*aspectRatio)));
    xCamFilmRes = aspectRatio * yCamFilmRes;
    
    
    
    //Now read in lens information from the spec file
    ifstream inFile(specfile.c_str());
    std::string line;
    while (std::getline(inFile, line))
    {
        std::istringstream iss(line);
        if(line.at(0)=='#')
            continue;
        lensData temp;
        iss >> temp.radius;
        iss >> temp.axisPos;
        iss >> temp.refractiveIndex;
        if(temp.refractiveIndex == 0.f)
            temp.refractiveIndex = 1.f;
        iss >> temp.aperture;
        lensInformation.push_back(temp);
        //Change for smallest diameter correction
        if(temp.radius == 0 && apertureDiameter < temp.aperture)
        {
            temp.aperture = apertureDiameter;
        }
        numLensInterface++;
    }
    inFile.close();
    
    //As per Profs Slides this is the last value
    lensInformation.back().axisPos = filmDistance;
    
    //Do a little more processing to get the values of different distances
    
    //Get the distance from lens to the film
    float d = 0.0f;
    for (int i = numLensInterface-1;i>=0;i--)
    {
        d += lensInformation.at(i).axisPos;
        lensInformation.at(i).distToFilm = d;
    }
    //And also the overall Z value of the film plane
    filmPlaneZ = -d;
    
    //Now get the center of each lens as we look from back to front
    for (int i = numLensInterface-1; i>=0; i--)
    {
        lensInformation.at(i).centerInCameraAxis = filmPlaneZ + lensInformation.at(i).distToFilm - lensInformation.at(i).radius;
    }
    
    //Finally Calulate the back lens area for weight calculation
    backLensArea = pow(lensInformation.back().aperture/2.f,2) * M_PI;
    
    std::cout << "[INFO]Lens Information read successfully !\n";
}

//Take a point sampled from screen space within (0,0) to (XRes,yRes) and
//return a point between (0,0) and (XCamRes,yCamRes) with z coordinated fixed at filmPlaneZ
Point RealisticCamera::RasterToCamera(Point &p) const
{
    float xc = xCamFilmRes-p.x*xCamFilmRes/(float)film->xResolution-xCamFilmRes/2.f;
    float yc = p.y * yCamFilmRes/(float)film->yResolution - yCamFilmRes/2.f;
    float zc = filmPlaneZ;
    return Point(xc, yc, zc);
}



RealisticCamera::~RealisticCamera()
{
    
}

float RealisticCamera::GenerateRay(const CameraSample &sample, Ray *ray) const
{
    
    //Size of lens vector
    int numLensInterface = lensInformation.size();
    

    //This is in raster space
    //Need to move this to the camera space before ray generation
    Point rasterSpaceOrg(sample.imageX,sample.imageY,0.0);
    Point camSpaceOrg;
    camSpaceOrg=RasterToCamera(rasterSpaceOrg);

    //Sample point on the last lens interface
    float lensU,lensV;
    ConcentricSampleDisk(sample.lensU,sample.lensV,&lensU,&lensV);
    
    //Aperture radius of the last lens in the lens system
    float lastApertureRad = (lensInformation.back().aperture)/2.f;
    
    //Use the sampling to determine a point on the first lens stop
    lensU *= lastApertureRad;
    lensV *= lastApertureRad;
    
    // z position for the back sampling disc
    float d = sqrtf( powf(lensInformation.back().radius, 2.f) - powf(lensInformation.back().aperture/2.f, 2.f));
    if (lensInformation.back().radius < 0.f)
        d = -d;
    float lensZCood = lensInformation.back().centerInCameraAxis + d;
    
    //The sampled point on the last image stop
    Point lensPoint(lensU,lensV,lensZCood);
   
    //Direction of the ray
    Vector dir = Normalize(lensPoint-camSpaceOrg);
    
    //Thats the ray we will trace throught the lens
    Ray startRay(camSpaceOrg,dir,0.f,INFINITY);
    
    
    //Start the ray tracing through the lens system
    Ray cumulativeRay;
    for(int i = numLensInterface-1;i>=0;i--)
    {
    
        Point centerOfLens(0,0,lensInformation.at(i).centerInCameraAxis);
        float curMedium = (lensInformation.at(i).refractiveIndex);
        float nextMedium;
        if(i!=0)
            nextMedium = (lensInformation.at(i-1).refractiveIndex);
        else
            nextMedium = 1; //Air after that
        

        float t;
        bool retVal = testIntersectionWithLensElement(startRay,lensInformation.at(i), &t);
        if(retVal)
        {
            Point pointOnLens = startRay(t);
            float lenghtofV = sqrt(pointOnLens.x*pointOnLens.x + pointOnLens.y*pointOnLens.y);
            if(lenghtofV > ((lensInformation.at(i).aperture)/(2.f)))
            {
                ray = NULL;
                return 0.0f;
            }
            bool retval2 = bendBySnellsLaw(startRay, pointOnLens, lensInformation.at(i), curMedium, nextMedium,&cumulativeRay);
            if(!retval2)
            {
                ray = NULL;
                return 0.0f;
            }
        }
        else
        {
            ray = NULL;
            return 0.0f;
        }
        startRay = cumulativeRay;
    }
    
    *ray = CameraToWorld(cumulativeRay);
    ray->d = Normalize(ray->d);
    //Calculate the ray weights
    //Use the back disc approximation for exit pupil 
    Vector filmNormal(0.f, 0.f, 1.f);
    Vector diskToFilm = Normalize(lensPoint - camSpaceOrg);
    float cosTheta = Dot(filmNormal, diskToFilm);
    return (backLensArea/pow(fabs(filmPlaneZ - lensPoint.z), 2.f)) * pow(cosTheta, 4.f);
    
}

bool testIntersectionWithLensElement( Ray r,lensData l ,float *t)
{
    //Check if surface is planar or not
    if(l.radius !=0.f)
    {
    
        //Determine the auxillary direction vector
        Vector D = r.o - Point(0,0,l.centerInCameraAxis);
    
        //Get the elements of the intersection equation
        float _a = Dot(r.d,r.d);
        float _b = 2 * Dot(D,r.d);
        float _c = Dot(D,D) - (l.radius*l.radius);
    
        //Calculate the discriminant
        float discriminant = (_b*_b) - (4*_a*_c);
    
        //If discriminant is negative, no intersection occurs.
        if(discriminant<0)
            return false;
    
        //Non negative discriminant, the ray intersect the lens
        float distSqrt = sqrt(discriminant);
    
        // compute t0 and t1
        float t0 = (-_b - distSqrt)/(2.0* _a );
        float t1 = (-_b + distSqrt)/(2.0* _a );
    
        // make sure t0 is smaller than t1
        if (t0 > t1)
        {
            // if t0 is bigger than t1 swap them around
            float temp = t0;
            t0 = t1;
            t1 = temp;
        }
        // Depending on the type of lens we return the value of t
        // Radius is negative -> Lens is convex when ray is from Film -> Smaller T value
        if(l.radius < 0)
        {
            *t = t0;
            return true;
        }
        else
        {
            *t = t1;
            return true;
        }
    }
    else
    {
        Vector normal = Vector(0.f,0.f,-1.f);
        *t = -(Dot(Vector(r.o), normal) + l.centerInCameraAxis) / Dot(r.d,normal);
        return true;
    }
   
}

bool bendBySnellsLaw(Ray r,Point p,lensData l,float n1,float n2,Ray *out)
{
    //Find incident and normal vector
    Vector r1_ = Normalize(r.d);
    Vector n_;
    //Could be spherical or planar... deal appropriately
    if(l.radius!=0.f)
        n_ = Normalize(p-Point(0,0,l.centerInCameraAxis));
    else
        n_ = Vector(0.f,0.f,-1.f);
    
    float mu = n1/n2;
    
    // Check if normals are in the right direction
    if(Dot(-r1_,n_)<0)
        n_ = -n_;
    
    //Now compute new direction using heckbert's method
    
    //Calculate directional cosines
    float cosTheta1 = -Dot(r1_,n_);
    float c2Squared = 1.f - ((mu*mu)*(1.f-(cosTheta1*cosTheta1)));
    if(c2Squared<0)
    {
        return false;
    }
    float c2 = sqrt(c2Squared);
    
    float gamma = mu*cosTheta1 - c2;
    
    //Find the new vector direction
    Vector ref_ = mu*r1_ + gamma*n_;
    
    //Construct a ray and return it
    Ray newRay(p,ref_,0.f,INFINITY);
    *out = newRay;
    return true;
    
}

RealisticCamera *CreateRealisticCamera(const ParamSet &params,
                                       const AnimatedTransform &cam2world, Film *film)
{
    // Extract common camera parameters from \use{ParamSet}
    float hither = params.FindOneFloat("hither", -1);
    float yon = params.FindOneFloat("yon", -1);
    float shutteropen = params.FindOneFloat("shutteropen", -1);
    float shutterclose = params.FindOneFloat("shutterclose", -1);
    
    // Realistic camera-specific parameters
    string specfile = params.FindOneString("specfile", "");
    float filmdistance = params.FindOneFloat("filmdistance", 70.0); // about 70 mm default to film
    float fstop = params.FindOneFloat("aperture_diameter", 1.0);
    float filmdiag = params.FindOneFloat("filmdiag", 35.0);
    string autofocusfile = params.FindOneString("af_zones", "");
    
    assert(hither != -1 && yon != -1 && shutteropen != -1 &&
           shutterclose != -1 && filmdistance!= -1);
    if (specfile == "") {
        Severe( "No lens spec file supplied!\n" );
    }
    return new RealisticCamera(cam2world, hither, yon,
                               shutteropen, shutterclose, filmdistance, fstop, 
                               specfile, autofocusfile, filmdiag, film);
}

