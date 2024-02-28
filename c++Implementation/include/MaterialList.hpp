#pragma once

#include "Material.hpp"
#include <vector>

class materialList
{
    public:
        materialList() : _materialList(nullptr), _materialListSize(0), _diffuseList(nullptr), _diffuseListSize(0), _globalIndex(0) {}
        void addMaterial(std::vector<MaterialInfo>& materialInfoList)
        {
            _materialList = new Material*[materialInfoList.size()];
            _diffuseList = new diffuseMaterial[materialInfoList.size()];
            for (size_t i = 0; i < materialInfoList.size(); i++)
            {
                _diffuseList[i] = diffuseMaterial(materialInfoList[i]._emission, materialInfoList[i]._specular, materialInfoList[i]._diffuse);
                _materialList[_globalIndex] = &_diffuseList[i];
                _materialListSize++;
                _diffuseListSize++;
                _globalIndex++;
            }
        }

        materialList(const materialList& other)
        {
            _materialList = new Material*[other._materialListSize];
            _diffuseList = new diffuseMaterial[other._diffuseListSize];
            _materialListSize = other._materialListSize;
            _diffuseListSize = other._diffuseListSize;
            _globalIndex = 0;
            for (size_t i = 0; i < other._diffuseListSize; i++)
            {
                _diffuseList[i] = other._diffuseList[i];
                _materialList[_globalIndex] = &_diffuseList[i];
                _globalIndex++;
            }
        }


        materialList& operator=(const materialList& other) = delete;

        ~materialList()
        {
            delete[] _diffuseList;
            delete[] _materialList;
        }

    private:
        Material** _materialList;
        size_t _materialListSize;
        diffuseMaterial* _diffuseList;
        size_t _diffuseListSize;
        size_t _globalIndex = 0;

};