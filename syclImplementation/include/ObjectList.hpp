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

    private:
    sycl::queue& _myQueue;
    size_t _objectListSize;
    GeometryList _geometryList;
    materialList _materialList;
    Object* _objectList = nullptr;

    public:
        inline size_t getObjectsListSize(){return _objectListSize;}
        
        //ObjectList() : _objectListSize(0) {}
        ObjectList(sycl::queue& myQueue) : _myQueue(myQueue), _objectListSize(0), _geometryList(myQueue), _materialList(myQueue){}
        void addObject(std::vector<Triangle> &tris, std::vector<MaterialInfo>& materialInfoList, std::vector<int>& geomIDs)
        {
            _geometryList.addObject(tris);
            _materialList.addMaterial(materialInfoList);
            size_t GeometryListSize = _geometryList.getGeometryListSize();
            //_objectList = new Object[GeometryListSize];
            _objectList = sycl::malloc_shared<Object>(GeometryListSize, _myQueue);

            for (size_t i = 0; i < GeometryListSize; i++)
            {
                

                _objectList[i]._materialIndex = static_cast<long>(geomIDs[i]);
                _objectList[i]._geometryIndex = static_cast<long>(i);

                _objectListSize++;  
            }

            

        }



        ObjectList(const ObjectList& other) = delete;
        ObjectList& operator=(const ObjectList& other) = delete;

        ~ObjectList()
        {
            if(_objectList != nullptr)
            {
                sycl::free(_objectList, _myQueue);
            }


        }

        Bounds3 getBounds(size_t index)
        {
            Object _object = _objectList[index];
            Geometry* _geometry = _geometryList.getGeometry(_object._geometryIndex);
            return _geometry->getBounds();
        }


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


        SamplingRecord Sample(RNG &rng, size_t index)
        {
            Object _object = _objectList[index];
            Geometry* _geometry = _geometryList.getGeometry(_object._geometryIndex);
            SamplingRecord record = _geometry->Sample(rng);

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


            Bounds3 centroidBounds;
            for (int i = left; i <= right; i++)
            {
                auto _geometry = _geometryList.getGeometry(_objectList[i]._geometryIndex);
                centroidBounds = Union(centroidBounds, _geometry->getBounds().Centroid());
            }
            int dim = centroidBounds.maxExtent();

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

        
        }

        bool spaceCompare(const Object& obja, const Object& objb, int dim)
        {

            auto a = _geometryList.getGeometry(obja._geometryIndex);
            auto b = _geometryList.getGeometry(objb._geometryIndex);


            float aCentroid = a->getBounds().Centroid()[dim];
            float bCentroid = b->getBounds().Centroid()[dim];

            return aCentroid < bCentroid;
        }


    sycl::queue& getQueue()
    {
        return _myQueue;
    }






};