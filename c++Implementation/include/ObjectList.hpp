#pragma once

#include "GeometryList.hpp"
#include "MaterialList.hpp"


struct Object
{
    long _geometryIndex = -1;
    long _materialIndex = -1;
};


class ObjectList
{
    GeometryList _geometryList;
    materialList _materialList;
    //size_t* _materialIndexArray = nullptr;
    Object* _objectList = nullptr;

    public:
        inline size_t getObjectsListSize(){return _objectListSize;}
        
        ObjectList() : _objectListSize(0) {}
        void addObject(std::vector<Triangle> &tris, std::vector<MaterialInfo>& materialInfoList, std::vector<int>& geomIDs)
        {
            _geometryList.addObject(tris);
            _materialList.addMaterial(materialInfoList);
            size_t GeometryListSize = _geometryList.getGeometryListSize();
            _objectList = new Object[GeometryListSize];
            for (size_t i = 0; i < GeometryListSize; i++)
            {
                //_materialIndexArray[i] = geomIDs[i];
                _objectList[i]._materialIndex = static_cast<long>(geomIDs[i]);
                _objectList[i]._geometryIndex = static_cast<long>(i);
                _objectListSize++;  
            }

        }

        ObjectList(const ObjectList& other) = delete;
        ObjectList& operator=(const ObjectList& other) = delete;

        ~ObjectList()
        {
            delete[] _objectList;
        }

        Bounds3 getBounds(size_t index)
        {
            Object _object = _objectList[index];
            Geometry* _geometry = _geometryList.getGeometry(_object._geometryIndex);
            return _geometry->getBounds();
        }

        //bool intersect(const Ray& ray){return _geometry->intersect(ray);}
        Intersection getIntersection(const Ray& ray, long index)
        {
            if (index < 0)
            {
                return Intersection();
            }
            Object _object = _objectList[index];
            auto _geometry = _geometryList.getGeometry(_object._geometryIndex);
            auto intersection = _geometry->getIntersection(ray);
            //intersection._material = _material;
            intersection._objectIndex = index;
            return intersection;
        }

        float getArea(long index)
        {
            if (index < 0)
            {
                return 0;
            }
            Object _object = _objectList[index];
            auto _geometry = _geometryList.getGeometry(_object._geometryIndex);
            return _geometry->getArea();
        }

        // Geometry* getGeometry(long index)
        // {

        //     if(index < 0)
        //     {
        //         std::cout << "Index is less than 0" << std::endl;
        //         return nullptr;
        //     }
        //     Object _object = _objectList[index];
        //     return _geometryList.getGeometry(_object._geometryIndex);
        // }

        SamplingRecord Sample(RNG &rng, size_t index)
        {
            Object _object = _objectList[index];
            Geometry* _geometry = _geometryList.getGeometry(_object._geometryIndex);
            SamplingRecord record = _geometry->Sample(rng);
            //auto _material = _materialList[_materialIndexArray[index]];
            auto _material = _materialList.getMaterial(_object._materialIndex);
            //record.pos._material = _material;
            record.pos._objectIndex = index;
            return record;
        }


        Material* getMaterial(long index)
        {
            if(index < 0)
            {
                return nullptr;
            }
            Object _object = _objectList[index];
            return _materialList.getMaterial(_object._materialIndex);
            //return _materialList.getMaterial(_materialIndexArray[index]);
        }




        void spacePartitioning(long left, long right)
        {

            //std::cout << left << "  " << right << std::endl;
            Bounds3 centroidBounds;
            for (int i = left; i <= right; i++)
            {
                auto _geometry = _geometryList.getGeometry(_objectList[i]._geometryIndex);
                centroidBounds = Union(centroidBounds, _geometry->getBounds().Centroid());
            }
            int dim = centroidBounds.maxExtent();
            // std::cout << dim << std::endl;
            // for (int i = left; i <= right; i++)
            // {
            //     auto _geometry = _geometryList.getGeometry(_objectList[i]._geometryIndex);
            //     std::cout << "ID number " << _objectList[i]._geometryIndex<< "  " <<_geometry->getBounds().Centroid()[dim] << std::endl;
            // }

            //std::cout << std::endl;
            if (dim == 0)
            {
                std::sort(_objectList + left, _objectList + right + 1 , [this](const Object& obja, const Object& objb) {
                    return spaceCompare(obja, objb, 0);
                });
            }
            else if (dim == 1)
            {
                std::sort(_objectList + left, _objectList + right + 1, [this](const Object& obja, const Object& objb) {
                    return spaceCompare(obja, objb, 1);
                });
            }
            else if (dim == 2)
            {
                std::sort(_objectList + left, _objectList + right + 1, [this](const Object& obja, const Object& objb) {
                    return spaceCompare(obja, objb, 2);
                });
            }
            // std::cout << "After sorting" << std::endl;
            // for (int i = left; i <= right; i++)
            // {
            //     auto _geometry = _geometryList.getGeometry(_objectList[i]._geometryIndex);
            //     std::cout << "ID number " << _objectList[i]._geometryIndex<< "  " <<_geometry->getBounds().Centroid()[dim] << std::endl;
            // }
        
        }

        bool spaceCompare(const Object& obja, const Object& objb, int dim)
        {

            auto a = _geometryList.getGeometry(obja._geometryIndex);
            auto b = _geometryList.getGeometry(objb._geometryIndex);


            float aCentroid = a->getBounds().Centroid()[dim];
            float bCentroid = b->getBounds().Centroid()[dim];

            return aCentroid < bCentroid;
        }







    private:

        size_t _objectListSize;

};