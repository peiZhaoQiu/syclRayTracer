#pragma once

#include "Geometry.hpp"
#include <vector>
#include "algorithm"


class GeometryList{

    private:

        sycl::queue& _myQueue;
        Geometry** _geometryList = nullptr;
        size_t _geometryListSize = 0;
        Triangle* _triangles = nullptr;
        size_t _trianglesSize = 0;
        size_t _globalIndex = 0;
        

    public:

        GeometryList() = delete;
        GeometryList(sycl::queue& myQueue) : _myQueue(myQueue),_geometryList(nullptr),_geometryListSize(0), _triangles(nullptr),_trianglesSize(0),_globalIndex(0){}

        void addObject(std::vector<Triangle> &tris)
        {
            //_triangles = new Triangle[tris.size()];
            _triangles = sycl::malloc_shared<Triangle>(tris.size(), _myQueue);
            //_geometryList = new Geometry*[tris.size()]; // Allocate memory for pointers to Triangles
            _geometryList = sycl::malloc_shared<Geometry*>(tris.size(), _myQueue);

            for (size_t i = 0; i < tris.size(); i++)
            {
                //_triangles[i] = tris[i];
                _myQueue.memcpy(_triangles+i, &tris[i], sizeof(Triangle)).wait();
                _geometryListSize++;
                _geometryList[_globalIndex] = &_triangles[i];
                _globalIndex++;
            }
            _trianglesSize = tris.size();
        }

        GeometryList(const GeometryList& other) = delete;



        GeometryList& operator=(const GeometryList& other) = delete;

        ~GeometryList()
        {


            if (_triangles != nullptr)
            {
                sycl::free(_triangles, _myQueue);
            }

            if(_geometryList != nullptr)
            {
                sycl::free(_geometryList, _myQueue);
            }

  
        }


        inline size_t getGeometryListSize(){return _geometryListSize;}


        inline Geometry* getGeometry(size_t index)
        {
            return _geometryList[index];
        }







        

};



