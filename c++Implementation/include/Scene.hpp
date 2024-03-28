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
// #include "BVH.hpp"
#include "OBJ_Loader.hpp"
#include "BVHArray.hpp"



class Scene
{

    private:

       ObjectList* _sceneObject = nullptr; 

    public:
        Scene()
        {
            _bvh = nullptr;
        }
        
        ~Scene()
        {
            if (_bvh != nullptr)
            {
                delete _bvh;
            }
        }

        Scene(ObjectList* sceneObject): _sceneObject(sceneObject)
        {
        }


 

        Scene(const Scene& scene) = delete;

        
        //BVHAccel *_bvh = nullptr;
        BVHArray *_bvh = nullptr;
        void buildBVH();

        SamplingRecord sampleLight(RNG &rng) const
        {

            size_t objectsListSize = _sceneObject->getObjectsListSize();   
                      
            float emitArea = 0;
            for (size_t i = 0; i < objectsListSize; i++)
            {
                Material* curMaterial = _sceneObject->getMaterial(i);
                if (curMaterial->getEmission())
                {
//                    emitArea += _objectsList[i]->getArea();
                      emitArea = _sceneObject->getArea(i);
                }
            }

            float p = std::abs(get_random_float(rng)) * emitArea;
            float area = 0;

            for (size_t i = 0; i < objectsListSize; i++)
            {
                Material* curMaterial = _sceneObject->getMaterial(i);
                if (curMaterial->getEmission())
                {
                    area = area + _sceneObject->getArea(i);
                    if (area >= p){
                    return _sceneObject->Sample(rng,i);
                        //pdf /= emitArea;
                        
                    }
                }
            }

            return SamplingRecord();


        }

        

        
        Vec3f doRendering(const Ray &initialRay, RNG &rng) const
        {
            Vec3f L_total = Vec3f(0, 0, 0);
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

                auto intersectionID = intersection._objectIndex;
                Material* intersectionMaterial = _sceneObject->getMaterial(intersectionID);

                if (intersectionMaterial->getEmission())
                {

                    dirLight[depth] = intersectionMaterial->getEmission();
                    break; // Terminate if the material has emission
                }


                auto samplingResult = sampleLight(rng);
                Intersection lightInter = samplingResult.pos;
                Material* lightInterMaterial = _sceneObject->getMaterial(lightInter._objectIndex);
                float lightPdf = samplingResult.pdf;

                Vec3f lightDir = (lightInter._position - intersection._position).normalized();
                float lightObjectdistance = (lightInter._position - intersection._position).length();
                Ray shadowRay(intersection._position, lightDir);
                Intersection shadowInter = castRay(shadowRay);

                if (shadowInter._hit && (shadowInter._position - lightInter._position).length() < 0.1f)
                {
                    L_dir = lightInterMaterial->_emission *
                            intersectionMaterial->eval(currentRay.direction, lightDir, intersection._normal) *
                            dotProduct(lightDir, intersection._normal) *
                            dotProduct(-lightDir, lightInter._normal) /
                            (lightObjectdistance * lightObjectdistance) /
                            lightPdf;
                }

                dirLight[depth] = L_dir;
                float russianRoulette = 0.8f;

                if (depth < 3 || get_random_float(rng) < russianRoulette)
                {
                    //outDirction = intersection._material->sample(currentRay.direction, intersection._normal,rng);
                    outDirction = intersectionMaterial->sample(currentRay.direction, intersection._normal,rng);
                    Ray outRay(intersection._position, outDirction);
                    Intersection outRayInter = castRay(outRay);
                    Material* outRayInterMaterial = _sceneObject->getMaterial(outRayInter._objectIndex);

                    if (outRayInter._hit && !outRayInterMaterial->getEmission())
                    {
                        LIndirParam = intersectionMaterial->eval(currentRay.direction, outDirction, intersection._normal) *
                                    dotProduct(outDirction, intersection._normal) /
                                    intersectionMaterial->pdf(currentRay.direction, outDirction, intersection._normal) /
                                    russianRoulette;
                    }
                }
                else
                {
                    indirLightParam[depth] = Vec3f(0, 0, 0);
                    break;
                }

                indirLightParam[depth] = LIndirParam;

                currentRay = Ray(intersection._position, outDirction); // Update the ray for the next iteration
            }


            for (int i = depth; i > 0; i--)
            {
                auto tem = (L_total + dirLight[i]); 
                L_total = tem * indirLightParam[i-1];
            }

            L_total = L_total * indirLightParam[0] + dirLight[0];
            return L_total;
        } 


        void commit()
        {
            std::cout << "building tree " << " object size " << _sceneObject->getObjectsListSize() <<std::endl;
            //this->_bvh = new BVHAccel(_sceneObject, _sceneObject->getObjectsListSize());
            this->_bvh = new BVHArray(_sceneObject);
            //std::cout << "The Tree size is  " << countTreeNodeSize(_bvh->root) <<std::endl;

        }

        // Intersection castRay(Ray inputRay) const
        // {
        //     Intersection result;
        //     float t;
        //     float t_min = INFINITY;
        //     size_t objectsListSize = _sceneObject->getObjectsListSize();
        //     for (size_t i = 0; i < objectsListSize; i++)
        //     {
        //         auto intersection = _sceneObject->getIntersection(inputRay,i);
        //         if(intersection._hit)
        //         {
        //             t = intersection._distance;
        //             if(t<t_min)
        //             {
        //                 t_min = t;
        //                 result = intersection;
        //             }
        //         }
        //     }
        //     return result;   
        // }


        Intersection castRay(const Ray& ray) const
        {
            return _bvh->Intersect(ray, _sceneObject);
        }


        int getObjectsListSize()
        {
            return _sceneObject->getObjectsListSize();
        }





};










