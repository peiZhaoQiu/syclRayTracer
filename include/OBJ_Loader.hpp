#pragma once

#include "tiny_obj_loader.h"
#include <string>
#include <vector>
#include "Material.hpp"
#include "Vec.hpp"
#include <iostream>
//#include "Triangle.hpp"
#include "Object.hpp"
#include <memory>


// struct Triangle_OBJ_result{
//     std::vector<Triangle> Triangles;
//     std::vector<MaterialInfo> MaterialsInfoList;
//     //std::vector<int> geomIDs;
//     std::vector<int> materialIDs;
// };

struct ObjectList
{
    Object** objectsList;
    Material** materialList;
    Geometry** geometryList;

    size_t objectsListSize = 0;
    size_t materialListSize = 0;
    size_t geometryListSize = 0;

};



class OBJ_Loader
{

    std::vector<Triangle> _gloabalTranglesResult;
    std::vector<MaterialInfo> _globalMaterialsInfoList;
    std::vector<int> _globalMaterialIDs;

    public:
    
    // Triangle_OBJ_result getTriangleResult()
    // {
    //     Triangle_OBJ_result Triangleresult;
    //     Triangleresult.Triangles = _gloabalTranglesResult;
    //     Triangleresult.MaterialsInfoList = _globalMaterialsInfoList;
    //     Triangleresult.materialIDs = _globalMaterialIDs;
    //     return Triangleresult;
    // }

    void addTriangleObjectFile(std::string objFilePath, std::string objFile)
    {

        //OBJ_result result;
        std::shared_ptr<tinyobj::ObjReader> readerPtr;

        int previousIDSize = _globalMaterialIDs.size();
        readerPtr = loadObjFile(objFilePath, objFile);   
        
        loadMaterial(readerPtr);

        auto& attrib = readerPtr->GetAttrib();
        auto& shapes = readerPtr->GetShapes();

        
        std::cout << "Load triangle size "<<shapes.size() << "  "<<std::endl;

        for (size_t i = 0; i<shapes.size(); i++) 
        {
            tinyobj::shape_t shape = shapes[i];
            std::cout << shape.name << std::endl;
            std::vector<Vec3f> vertices;

            for (size_t i = 0;i<attrib.vertices.size();i+=3)
            {
                float vx = attrib.vertices[i];
                float vy = attrib.vertices[i+1];
                float vz = attrib.vertices[i+2];
                vertices.push_back(Vec3f(vx,vy,vz));
            }

            for (size_t i = 0; i < shape.mesh.indices.size(); i += 3)
            {
                unsigned int index1 = shape.mesh.indices[i].vertex_index;
                unsigned int index2 = shape.mesh.indices[i + 1].vertex_index;
                unsigned int index3 = shape.mesh.indices[i + 2].vertex_index;

                Triangle tri(vertices[index1], vertices[index2], vertices[index3]);
                _gloabalTranglesResult.push_back(tri);
            }

            //Triangle tri(vertices[0],vertices[1],vertices[2]);
            //result.Triangles.push_back(tri);

            
        
            for (auto &id : shape.mesh.material_ids)
            {

                std::cout << id << "  " << std::endl;
                _globalMaterialIDs.push_back(previousIDSize + id);
                //_result.materialIDs.push_back(previousSize + id);
            }

           // _result.materialIDs = _globalMaterialIDs;
        }
        std::cout << "Loaded " << objFilePath + objFile << " have " << _gloabalTranglesResult.size() << " Triangles in total."<<std::endl;
        //return result;
    }

    std::shared_ptr<ObjectList> outputObj();

    private:

   // std::shared_ptr<MyType> myFunction() {
    // auto obj = std::make_shared<MyType>();


    std::shared_ptr<tinyobj::ObjReader> loadObjFile(std::string objFilePath, std::string objFile)
    {
        std::string inputfile = objFilePath + objFile;
        tinyobj::ObjReaderConfig reader_config;
        reader_config.mtl_search_path = objFilePath; // Path to material files
        auto reader = std::make_shared<tinyobj::ObjReader>();
        // tinyobj::ObjReader reader;

        if (!reader->ParseFromFile(inputfile, reader_config)) 
        {
            if (!reader->Error().empty()) 
            {
                std::cerr << "TinyObjReader: " << reader->Error();
            }
            //return result;
        }

        if (!reader->Warning().empty()) 
        {
            std::cout << "TinyObjReader: " << reader->Warning();
        }
        std::cout << "Loading " << inputfile << std::endl;
        return reader;

    }


    void loadMaterial(std::shared_ptr<tinyobj::ObjReader> readerPtr)
    {
        std::cout << "here" << std::endl;
        auto materials = readerPtr->GetMaterials();
        std::cout << " load material size " <<materials.size() << std::endl;
        for(size_t i = 0; i< materials.size();i++)
        {
            //std::cout << materials[0].diffuse << " " << std::endl;
            Vec3f diffuseVec(materials[i].diffuse[0],materials[i].diffuse[1],materials[i].diffuse[2]);
            Vec3f specularVec(materials[i].specular[0],materials[i].specular[1],materials[i].specular[2]);
            Vec3f emissionVec(materials[i].emission[0],materials[i].emission[1],materials[i].emission[2]);
            MaterialInfo mat = MaterialInfo(emissionVec,specularVec,diffuseVec);
            //_result.MaterialsInfoList.push_back(mat);
            _globalMaterialsInfoList.push_back(mat);

            std::cout << materials[i].emission[0] << " " << materials[i].emission[1] << " " << materials[i].emission[2] << std::endl;
            std::cout << materials[i].diffuse[0] << " " << materials[i].diffuse[1] << " " << materials[i].diffuse[2] << std::endl;
            std::cout << std::endl;
        }

    }



};






std::shared_ptr<ObjectList> OBJ_Loader::outputObj()
{
    //OBJ_result result = OBJ_Loader::addObject(objFilePath, objFile);
    //std::shared_ptr<ObjectList> result;
    auto result = std::make_shared<ObjectList>();

    result->objectsListSize = _gloabalTranglesResult.size();
    result->materialListSize = _globalMaterialsInfoList.size();
    result->geometryListSize = _gloabalTranglesResult.size();

    result->objectsList = new Object*[result->objectsListSize];
    result->materialList = new Material*[result->materialListSize];
    result->geometryList = new Geometry*[result->geometryListSize];


    
    for (size_t i = 0;i<result->geometryListSize;i++)
    {
        Triangle* triPtr = new Triangle(_gloabalTranglesResult[i]._v1,_gloabalTranglesResult[i]._v2,_gloabalTranglesResult[i]._v3);
        result->geometryList[i] = triPtr;
    }
    


    for (size_t i = 0;i<result->materialListSize;i++)
    {
        MaterialInfo matInfo = _globalMaterialsInfoList[i];
        Material* materialPtr = new diffuseMaterial(matInfo._emission,matInfo._specular,matInfo._diffuse);
        
        
        //MaterialFactory::createMaterial(matInfo._emission,matInfo._specular,matInfo._diffuse);
        result->materialList[i] = materialPtr;
    }

    for (size_t i = 0;i<result->objectsListSize;i++)
    {
        Object* objPtr = new Object(result->geometryList[i],result->materialList[_globalMaterialIDs[i]]);
        std::cout << "material id " << _globalMaterialIDs[i] << std::endl;
        result->objectsList[i] = objPtr;
    }

    return result;
    // for(size_t i = 0; i< result.Triangles.size();i++)
    // {

    //     Triangle *triPtr = new Triangle(result.Triangles[i]._v1, result.Triangles[i]._v2, result.Triangles[i]._v3);
    //     Object *new_obj = new Object(triPtr,materialPtrList[result.materialIDs[i]]);
    //     _objectsList.emplace_back(new_obj);
    // }  

}