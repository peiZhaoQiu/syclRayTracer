#pragma once

#include "OBJ_Loader.hpp"


class GeometryList{

    public: 
        GeometryList(){};
        Geometry** _geometryList = nullptr;
        size_t _geometryListSize = 0;
        Triangle* _triangles = nullptr;
        size_t _trianglesSize = 0;
        void addTriangle(std::vector<Triangle> tris)
        {
            _triangles = new Triangle[tris.size()];
            for (size_t i = 0; i < tris.size(); i++)
            {
                _triangles[i] = tris[i];
                _geometryListSize++;
                _geometryList[_globalIndex] = &_triangles[i];
                _globalIndex++;
            }
            _trianglesSize = tris.size();
        }
    private:
        size_t _globalIndex = 0;
};



