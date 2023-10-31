#pragma once

#include "Vec.hpp"
#include "Ray.hpp"
#include "Intersection.hpp"
#include "Material.hpp"
#include "Object.hpp"
#include <vector>   
#include <string>
#include <cmath>
#include <iostream>
#include "BVH.hpp"



class Scene
{
    public:
        Scene()
        {


            _objectsList = nullptr;
            _materialList = nullptr;
            _geometryList = nullptr;
            _bvh = nullptr;

            _objectsListSize = 0;
            _materialListSize = 0;
            _geometryListSize = 0;

        }
        
        ~Scene()
        {
            //std::cout << "Scene destructor called" << std::endl;
            if (_bvh != nullptr){
                delete _bvh;
                _bvh = nullptr;
            }
            
            if(_materialList != nullptr){
                for (size_t i = 0; i < _materialListSize; i++)
                {
                    if (_materialList[i] != nullptr){
                        delete _materialList[i];
                    }
                    
                }
                delete _materialList;
                _materialList = nullptr;
            }

            if (_geometryList != nullptr){
                for (size_t i = 0; i < _geometryListSize; i++)
                {
                    if (_geometryList[i] != nullptr){
                        delete _geometryList[i];
                    }
                
                }  
                delete _geometryList;
                _geometryList = nullptr;
            }

            if(_objectsList != nullptr){
                for (size_t i = 0; i < _objectsListSize; i++)
                {
                    if (_objectsList[i] != nullptr){
                        delete _objectsList[i];
                    }
                    
                }
                delete _objectsList;
                _objectsList = nullptr;
            }
        }

        Scene(Object** objectsList, Material** materialList, Geometry** geometryList, size_t objectsListSize, size_t materialListSize, size_t geometryListSize): _objectsList(objectsList), _materialList(materialList), _geometryList(geometryList), _objectsListSize(objectsListSize), _materialListSize(materialListSize), _geometryListSize(geometryListSize) {}
 

        Scene(const Scene& scene)
        {

            for (size_t i = 0; i < scene._materialListSize; i++)
            {
                delete _materialList[i];
            }

            for (size_t i = 0; i < scene._geometryListSize; i++)
            {
                delete _geometryList[i];
            }

            for (size_t i = 0; i < scene._objectsListSize; i++)
            {
                delete _objectsList[i];
            }

            delete _bvh;

            _objectsList = scene._objectsList;
            _materialList = scene._materialList;
            _geometryList = scene._geometryList;
            

            _objectsListSize = scene._objectsListSize;
            _materialListSize = scene._materialListSize;
            _geometryListSize = scene._geometryListSize;
            _bvh = new BVHAccel(_objectsList, _objectsListSize);
        }

        Intersection castRay(Ray inputRay) const;

    //void addMeshObj(std::string objFilePath, std::string objFile);
    //void addTriangleObjFile(OBJ_Loader& loader);

    //std::vector<Object> _objectsList;
    //std::vector<Material> _materialList;
    //std::vector<Geometry> _geometryList;

        Object** _objectsList;
        Material** _materialList;
        Geometry** _geometryList;

        size_t _objectsListSize;
        size_t _materialListSize;
        size_t _geometryListSize;


        BVHAccel *_bvh = nullptr;
        void buildBVH();

        SamplingRecord sampleLight(RNG &rng) const
        {
            //bool first = true;
            float emitArea = 0;
            // if (first){
                for (size_t i = 0; i < _objectsListSize; i++){
                    if (_objectsList[i]->_material->hasEmission()){
                        emitArea += _objectsList[i]->getArea();
                    }
                }
            //     first = false;
            // }

            float p = std::abs(get_random_float(rng)) * emitArea;
            float area = 0;
            for (size_t i = 0; i < _objectsListSize; i++)
            {
                if (_objectsList[i]->_material->hasEmission())
                {
                    area = area + _objectsList[i]->getArea();
                    if (area >= p){
                    return _objectsList[i]->Sample(rng);
                        //pdf /= emitArea;
                        
                    }
                }
            }


        }

        

        
        Vec3f doRendering(const Ray &initialRay, RNG &rng) const
        {
            Vec3f L_total = Vec3f(0, 0, 0);
            //Vec3f L_indir = Vec3f(0, 0, 0);
            Ray currentRay = initialRay;
            int maxDepth = 50;

            Vec3f indirLightParam[50];
            Vec3f dirLight[50];

            for (int i = 0; i < maxDepth; i++)
            {
                indirLightParam[i] = Vec3f(0, 0, 0);
                dirLight[i] = Vec3f(0, 0, 0);
            }

            int depth = 0;
            
            for (depth = 0; depth < maxDepth; ++depth)
            {
                Vec3f L_dir = Vec3f(0, 0, 0);
                Vec3f LIndirParam = Vec3f(0, 0, 0);
                Vec3f outDirction;
                Intersection intersection = castRay(currentRay);

                if (!intersection._hit)
                {
                    break; // Terminate if no intersection
                }

                if (intersection._material->hasEmission())
                {
                    dirLight[depth] = intersection._material->_emission;
                    break; // Terminate if the material has emission
                }


                auto samplingResult = sampleLight(rng);
                Intersection lightInter = samplingResult.pos;
                float lightPdf = samplingResult.pdf;

                Vec3f lightDir = (lightInter._position - intersection._position).normalized();
                float lightObjectdistance = (lightInter._position - intersection._position).length();
                Ray shadowRay(intersection._position, lightDir);
                Intersection shadowInter = castRay(shadowRay);

                if (shadowInter._hit && (shadowInter._position - lightInter._position).length() < 0.1f)
                {
                    L_dir = lightInter._material->_emission *
                            intersection._material->eval(currentRay.direction, lightDir, intersection._normal) *
                            dotProduct(lightDir, intersection._normal) *
                            dotProduct(-lightDir, lightInter._normal) /
                            (lightObjectdistance * lightObjectdistance) /
                            lightPdf;
                }

                dirLight[depth] = L_dir;

                if (depth < 3 || get_random_float(rng) < 0.8f)
                {
                    outDirction = intersection._material->sample(currentRay.direction, intersection._normal,rng);
                    Ray outRay(intersection._position, outDirction);
                    Intersection outRayInter = castRay(outRay);

                    if (outRayInter._hit && !outRayInter._material->hasEmission())
                    {
                        LIndirParam = intersection._material->eval(currentRay.direction, outDirction, intersection._normal) *
                                    dotProduct(outDirction, intersection._normal) /
                                    intersection._material->pdf(currentRay.direction, outDirction, intersection._normal) /
                                    0.8f;
                    }
                }
                else
                {
                    indirLightParam[depth] = Vec3f(0, 0, 0);
                    break;
                }

                //L_indir = LIndirParam;
                indirLightParam[depth] = LIndirParam;
                //L_total += L_dir + L_indir;

                currentRay = Ray(intersection._position, outDirction); // Update the ray for the next iteration
            }


            for (int i = depth; i > 0; i--)
            {
                //L_indir = indirLightParam[i] + dirLight[i] * L_indir;
                auto tem = (L_total + dirLight[i]); 
                L_total = tem * indirLightParam[i-1];
            }

            L_total = L_total * indirLightParam[0] + dirLight[0];
            return L_total;
        } 


        void commit(); 

};










