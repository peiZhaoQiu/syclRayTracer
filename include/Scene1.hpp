#pragma once
#include "Scene.hpp"
#include <string>
#include <unordered_map>
#include "OBJ_Loader.hpp"

//void Scene::buildBVH() {
    //printf(" - Generating BVH...\n\n");
    //this->_bvh = new BVHAccel(_objectsList, 1, BVHAccel::SplitMethod::NAIVE);
//}


Intersection Scene::castRay(Ray inputRay) const
{
    Intersection result;


    if (this->_bvh != nullptr){
        result = this->_bvh->Intersect(inputRay);
    }

     //   result = this->_bvh->Intersect(inputRay);

    return result;
}

// Intersection Scene::castRay(Ray inputRay) const
// {
//     Intersection result;
//     float t;
//     float t_min = INFINITY;
//     for (size_t i = 0; i < _objectsListSize; i++)
//     {
//         auto intersection = _objectsList[i]->getIntersection(inputRay);
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


void Scene::commit()
{
    std::cout << "building tree " << " object size " << _objectsListSize <<std::endl;
    this->_bvh = new BVHAccel(_objectsList, _objectsListSize);
}

