#pragma once

#include "tiny_obj_loader.h"
#include <string>
#include <vector>
#include "Material.hpp"
#include "Vec.hpp"
#include <iostream>
#include "Object.hpp"
#include <memory>
#include "ObjectList.hpp"





// struct Triangle_OBJ_result{
//     std::vector<Triangle> Triangles;
//     std::vector<MaterialInfo> MaterialsInfoList;
//     //std::vector<int> geomIDs;
//     std::vector<int> materialIDs;
// };

//struct SceneObjects
{
    //Object** objectsList = nullptr;
    //Material** materialList = nullptr;
    //Geometry** geometryList = nullptr;
    //GeometryList geometryList;
    //ObjectList objectList;

    //size_t objectsListSize = 0;
    //size_t materialListSize = 0;
    //size_t geometryListSize = 0;

};



class OBJ_Loader
{

    std::vector<Triangle> _gloabalTranglesResult;
    std::vector<MaterialInfo> _globalMaterialsInfoList;
    std::vector<int> _globalMaterialIDs;

    public:
    

    void addTriangleObjectFile(std::string objFilePath, std::string objFile)
    {

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


            
        
            for (auto &id : shape.mesh.material_ids)
            {

                _globalMaterialIDs.push_back(previousIDSize + id);
            }

        }
        std::cout << "Loaded " << objFilePath + objFile << " have " << _gloabalTranglesResult.size() << " Triangles in total."<<std::endl;
    }

    std::shared_ptr<ObjectList> outputObj();

    private:

    std::shared_ptr<tinyobj::ObjReader> loadObjFile(std::string objFilePath, std::string objFile)
    {
        std::string inputfile = objFilePath + objFile;
        tinyobj::ObjReaderConfig reader_config;
        reader_config.mtl_search_path = objFilePath; // Path to material files
        auto reader = std::make_shared<tinyobj::ObjReader>();

        if (!reader->ParseFromFile(inputfile, reader_config)) 
        {
            if (!reader->Error().empty()) 
            {
                std::cerr << "TinyObjReader: " << reader->Error();
            }
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
        auto materials = readerPtr->GetMaterials();
        std::cout << " load material size " <<materials.size() << std::endl;
        for(size_t i = 0; i< materials.size();i++)
        {
            Vec3f diffuseVec(materials[i].diffuse[0],materials[i].diffuse[1],materials[i].diffuse[2]);
            Vec3f specularVec(materials[i].specular[0],materials[i].specular[1],materials[i].specular[2]);
            Vec3f emissionVec(materials[i].emission[0],materials[i].emission[1],materials[i].emission[2]);
            MaterialInfo mat = MaterialInfo(emissionVec,specularVec,diffuseVec);
            _globalMaterialsInfoList.push_back(mat);

        }

    }



};






std::shared_ptr<ObjectList> OBJ_Loader::outputObj()
{
    auto result = std::make_shared<ObjectList>();

    result->addObject(_gloabalTranglesResult,_globalMaterialsInfoList,_globalMaterialIDs);

    return result;

}

