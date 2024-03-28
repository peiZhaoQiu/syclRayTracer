#pragma once

#include "Geometry.hpp"
#include <vector>
#include "algorithm"


class GeometryList{

    public:

        GeometryList() : _geometryList(nullptr), _geometryListSize(0), _triangles(nullptr), _trianglesSize(0), _globalIndex(0) {}
        void addObject(std::vector<Triangle> &tris)
        {
            _triangles = new Triangle[tris.size()];
            _geometryList = new Geometry*[tris.size()]; // Allocate memory for pointers to Triangles

            for (size_t i = 0; i < tris.size(); i++)
            {
                _triangles[i] = tris[i];
                _geometryListSize++;
                _geometryList[_globalIndex] = &_triangles[i];
                _globalIndex++;
            }
            _trianglesSize = tris.size();
        }

        GeometryList(const GeometryList& other)
        {
            _geometryList = new Geometry*[other._geometryListSize];
            _triangles = new Triangle[other._trianglesSize];
            _geometryListSize = other._geometryListSize;
            _trianglesSize = other._trianglesSize;
            _globalIndex = 0;
            for (size_t i = 0; i < other._trianglesSize; i++)
            {
                _triangles[i] = other._triangles[i];
                _geometryList[_globalIndex] = &_triangles[i];
                _globalIndex++;
            }
        }


        GeometryList& operator=(const GeometryList& other) = delete;

        ~GeometryList()
        {
            delete[] _triangles;
            delete[] _geometryList;
        }


        inline size_t getGeometryListSize(){return _geometryListSize;}


        inline Geometry* getGeometry(size_t index)
        {
            return _geometryList[index];
        }






    private:
        Geometry** _geometryList;
        size_t _geometryListSize;
        Triangle* _triangles;
        size_t _trianglesSize;
        size_t _globalIndex = 0;
        

};



