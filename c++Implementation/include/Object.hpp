#pragma once
#include "Geometry.hpp"
#include "Material.hpp"
#include <iostream>

class Object
{
public:
    Object(){};
    ~Object(){
        // std::cout << "Object destructor called" << std::endl;
        // if (_geometry != nullptr){
        //     delete _geometry;
        // }

        // if (_material != nullptr){
        //     delete _material;
        // }
    };
    Geometry* _geometry;
    Material* _material;
    Object(Geometry* geometry, Material* material): _geometry(geometry), _material(material) {}
    Bounds3 getBounds(){return _geometry->getBounds();}

    //bool intersect(const Ray& ray){return _geometry->intersect(ray);}
    Intersection getIntersection(const Ray& ray)
    {
        auto intersection = _geometry->getIntersection(ray);
        intersection._material = _material;
        return intersection;
    }
    float getArea(){return _geometry->getArea();}
    SamplingRecord Sample(RNG &rng)
    {
        SamplingRecord record = _geometry->Sample(rng);
        record.pos._material = _material;
        return record;
    }


};