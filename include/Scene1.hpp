#pragma once
#include "Scene.hpp"
#include <string>
#include <unordered_map>
#include "OBJ_Loader.hpp"

Scene::Scene()
{
    //_materialList = std::vector<Material*>();
    //_objectsList = std::vector<Object*>();

    _objectsList = nullptr;
    _materialList = nullptr;
    _geometryList = nullptr;

    _objectsListSize = 0;
    _materialListSize = 0;
    _geometryListSize = 0;

}


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    //this->_bvh = new BVHAccel(_objectsList, 1, BVHAccel::SplitMethod::NAIVE);
}


// Intersection Scene::castRay(Ray inputRay)
// {
//     Intersection result;

//     if (this->_bvh == nullptr){
//         printf(" - BVH not built, building...\n");
//         this->buildBVH();
//     }

//     if (this->_bvh != nullptr){
//         result = this->_bvh->Intersect(inputRay);
//     }

//     return result;
// }

Intersection Scene::castRay(Ray inputRay) const
{
    Intersection result;
    float t;
    float t_min = INFINITY;
    for (size_t i = 0; i < _objectsListSize; i++)
    {
        auto intersection = _objectsList[i]->getIntersection(inputRay);
        if(intersection._hit)
        {
            t = intersection._distance;
            if(t<t_min)
            {
                t_min = t;
                result = intersection;
            }
        }
    }
    return result;   
}


//void Scene::addMeshObj(std::string objFilePath, std::string objFile)
// void Scene::addTriangleObjFile(OBJ_Loader& loader)
// {
//     //OBJ_result result = OBJ_Loader::addObject(objFilePath, objFile);

//     Triangle_OBJ_result TriangleResult = loader.getTriangleResult();
    



//     std::vector<Material*> materialPtrList;

//     for (size_t i = 0;i<TriangleResult.MaterialsInfoList.size();i++)
//     {

//         //Material(Vec3f emission, Vec3f specular, Vec3f diffuse);
//         //Material* materialPtr = new Material(result.Materials[i]._emission,result.Materials[i]._specular,result.Materials[i]._diffuse);

//         //materialPtrList.emplace_back(materialPtr);
//         MaterialInfo matInfo = result.MaterialsInfoList[i];
//         Material* materialPtr = MaterialFactory::createMaterial(matInfo._emission,matInfo._specular,matInfo._diffuse);
//         materialPtrList.emplace_back(materialPtr);
//         _materialList.emplace_back(materialPtr); 
//     }

//     for(size_t i = 0; i< result.Triangles.size();i++)
//     {

//         Triangle *triPtr = new Triangle(result.Triangles[i]._v1, result.Triangles[i]._v2, result.Triangles[i]._v3);
//         Object *new_obj = new Object(triPtr,materialPtrList[result.materialIDs[i]]);
//         _objectsList.emplace_back(new_obj);
//     }  

// }