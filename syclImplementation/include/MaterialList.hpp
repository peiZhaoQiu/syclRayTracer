#pragma once

#include "Material.hpp"
#include <vector>

class materialList
{
    private:
        sycl::queue& _myQueue;   
        Material** _materialList = nullptr;
        size_t _materialListSize = 0;
        diffuseMaterial* _diffuseList = nullptr;
        size_t _diffuseListSize = 0;
        size_t _globalIndex = 0;
         
    public:
        materialList(sycl::queue& myQueue) :_myQueue(myQueue), _materialList(nullptr), _materialListSize(0), _diffuseList(nullptr),_diffuseListSize(0),_globalIndex(0) {}
        void addMaterial(std::vector<MaterialInfo>& materialInfoList)
        {
            _materialList = sycl::malloc_shared<Material*>(materialInfoList.size(), _myQueue);
            _diffuseList = sycl::malloc_shared<diffuseMaterial>(materialInfoList.size(), _myQueue);
            for (size_t i = 0; i < materialInfoList.size(); i++)
            {
                //_diffuseList[i] = diffuseMaterial(materialInfoList[i]._emission, materialInfoList[i]._specular, materialInfoList[i]._diffuse);
                diffuseMaterial material(materialInfoList[i]._emission, materialInfoList[i]._specular, materialInfoList[i]._diffuse);
                _myQueue.memcpy(_diffuseList+i, &material, sizeof(diffuseMaterial)).wait();
                _materialList[_globalIndex] = &_diffuseList[i];
                _materialListSize++;
                _diffuseListSize++;
                _globalIndex++;
            }
        }

        materialList(const materialList& other) = delete;
        // {
        //     _materialList = new Material*[other._materialListSize];
        //     _diffuseList = new diffuseMaterial[other._diffuseListSize];
        //     _materialListSize = other._materialListSize;
        //     _diffuseListSize = other._diffuseListSize;
        //     _globalIndex = 0;
        //     for (size_t i = 0; i < other._diffuseListSize; i++)
        //     {
        //         _diffuseList[i] = other._diffuseList[i];
        //         _materialList[_globalIndex] = &_diffuseList[i];
        //         _globalIndex++;
        //     }
        // }


        materialList& operator=(const materialList& other) = delete;


        inline Material* getMaterial(long index)
        {
            if(index >= _materialListSize || index < 0)
            {
                return nullptr;
            }

            return _materialList[index];
        }

        ~materialList()
        {
            //delete[] _diffuseList;
            if (_diffuseList != nullptr)
            {
                free(_diffuseList, _myQueue);
            }
            //delete[] _materialList;
            if (_materialList != nullptr)
            {
                free(_materialList, _myQueue);
            }
        }



};