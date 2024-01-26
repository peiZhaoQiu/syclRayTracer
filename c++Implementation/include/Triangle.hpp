#pragma once
#include "Geometry.hpp"
#include "common.hpp"

class Geometry;
class Triangle : public Geometry
{
    public:
         
        Vec3f _v1,_v2,_v3;
        Triangle()
            : Geometry(GeometryType::TRIANGLE){}
        Triangle (const Vec3f &v1,const Vec3f &v2, const Vec3f &v3):_v1(v1),_v2(v2),_v3(v3),Geometry(GeometryType::TRIANGLE)
        {
            e1 = _v2 - _v1;
            e2 = _v3 - _v1;
            normal = crossProduct(e1,e2).normalized();
            area = (crossProduct(e1,e2)).length()*0.5f;
            Geometry::_type = GeometryType::TRIANGLE;
        }
        
       // ~Triangle() = default;

    //bool Intersect_virtual(const Ray& ray) const;
    Intersection getIntersection_virtual(const Ray& ray) const;
        
         
    float getArea_virtual()const{
        return this->area;
    }

    SamplingRecord Sample_virtual(RNG &rng)
    {
        SamplingRecord record;
        record.pdf = 1.0f / area;
        float x = get_random_float(rng);
        float y = get_random_float(rng);
        record.pos._position = _v1 * (1.0f - x) + _v2 * (x * (1.0f - y)) + _v3 * (x * y);
        record.pos._normal = this->normal;
        return record;
    }


    Bounds3 getBounds_virtual() const
    {
         return Union(Bounds3(_v1, _v2), _v3); 
    }
    

    private:

        float area;
        Vec3f normal;
        Vec3f e1,e2;

};


Intersection Triangle::getIntersection_virtual(const Ray& ray) const
{
    Intersection intersection;
    if(dotProduct(normal,ray.direction) > 0)
    {
        intersection._hit = false;
        return intersection;
    }

    float u,v,t_tmp = 0;
    Vec3f pvec = crossProduct(ray.direction,e2);
    float det = dotProduct(e1,pvec);
    if(fabs(det) < MyEPSILON)
    {
        intersection._hit = false;
        return intersection;
    }
    float inv_det = 1.0f / det;
    Vec3f tvec = ray.origin - _v1;
    u = dotProduct(tvec,pvec)*inv_det;
    if(u < 0 || u > 1)
    {
        intersection._hit = false;
        return intersection;
    }
    Vec3f qvec = crossProduct(tvec,e1);
    v = dotProduct(ray.direction,qvec)*inv_det ;
    if(v < 0 || u + v > 1)
    {
        intersection._hit = false;
        return intersection;
    }



    t_tmp = dotProduct(e2,qvec) * inv_det;

    if(t_tmp < MyEPSILON)
    {
        intersection._hit = false;
        return intersection;
    }

    intersection._hit = true;
    
    intersection._position = ray.origin + ray.direction * t_tmp;
    intersection._normal = normal;
    intersection._distance = t_tmp;
    return intersection;


}



