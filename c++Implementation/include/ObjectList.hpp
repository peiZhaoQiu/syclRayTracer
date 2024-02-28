#pragma once

#include "GeometryList.hpp"
#include "MaterialList.hpp"


class ObjectList
{
    GeometryList _geometryList;
    materialList _materialList;
    size_t* _materialIndexArray = nullptr;


    public:
        inline size_t getObjectsListSize(){return _objectListSize;}
        
        ObjectList() : _objectListIndex(nullptr), _objectListSize(0) {}
        void addObject(std::vector<Triangle> &tris, std::vector<MaterialInfo>& materialInfoList, std::vector<int>& geomIDs)
        {
            _geometryList.addObject(tris);
            _materialList.addMaterial(materialInfoList);
            __materialIndexArray = new size_t[tris.size()];
            size_t GeometryListSize = _geometryList.getGeometryListSize();
            for (size_t i = 0; i < GeometryListSize; i++)
            {
                _materialIndexArray[i] = geomIDs[i];
                _objectListSize++;  
            }

        }

        ObjectList(const ObjectList& other) = delete;
        ObjectList& operator=(const ObjectList& other) = delete;

        ~ObjectList()
        {
            delete[] _materialIndexArray;
        }

        Bounds3 getBounds(size_t index)
        {
            auto _geometry = _geometryList[index];
            return _geometry->getBounds();
        }

        //bool intersect(const Ray& ray){return _geometry->intersect(ray);}
        Intersection getIntersection(const Ray& ray, size_t index)
        {
            auto _geometry = _geometryList[index];
            auto intersection = _geometry->getIntersection(ray);
            //intersection._material = _material;
            intersection._objectIndex = index;
            return intersection;
        }

        float getArea(size_t index)
        {
            auto _geometry = _geometryList[index];
            return _geometry->getArea();
        }

        SamplingRecord Sample(RNG &rng, size_t index)
        {
            auto _geometry = _geometryList[index];
            SamplingRecord record = _geometry->Sample(rng);
            auto _material = _materialList[_materialIndexArray[index]];
            //record.pos._material = _material;
            record.pos._objectIndex = index;
            return record;
        }


        ~ObjectList()
        {
            delete[] _materialIndexArray;
        }

    private:

        size_t _objectListSize;

};