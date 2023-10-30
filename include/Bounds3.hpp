#pragma once


#include "Ray.hpp"
#include "Vec.hpp"
#include <limits>
#include <array>
#include <cmath>
#include <float.h>

class Bounds3
{
    public:
        Vec3f pMin,pMax;
        Bounds3()
        {
            double minNum = std::numeric_limits<double>::lowest();
            double maxNum = std::numeric_limits<double>::max();
            pMax = Vec3f(minNum,minNum,minNum);
            pMin = Vec3f(maxNum,maxNum,maxNum);
        }

        Bounds3(const Vec3f p): pMin(p), pMax(p) {}
        Bounds3(const Vec3f p1, const Vec3f p2)
        {
            pMin = Vec3f( fmin(p1.x, p2.x), fmin(p1.y, p2.y), fmin(p1.z, p2.z) );
            pMax = Vec3f( fmax(p1.x, p2.x), fmax(p1.y, p2.y), fmax(p1.z, p2.z) );
        }

        Vec3f Diagonal () const {return pMax - pMin;}
        int maxExtent () const 
        {

            Vec3f d = Diagonal();
            if (d.x > d.y && d.x > d.z){return 0;}
            else if (d.y >d.z){return 1;}
            else {return 2;}
        }

        double SurfaceArea() const
        {
            Vec3f d = Diagonal();
            return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
        }

        Vec3f Centroid() {return 0.5 * pMin + 0.5 * pMax;}
        Bounds3 Intersect(const Bounds3& b)
        {
            return Bounds3(
                Vec3f( fmax(pMin.x, b.pMin.x), fmax(pMin.y, b.pMin.y),fmax(pMin.z, b.pMin.z) ),
                Vec3f( fmin(pMax.x, b.pMax.x), fmin(pMax.y, b.pMax.y),fmin(pMax.z, b.pMax.z) )
            );
        }

        bool Overlaps(const Bounds3& b1, const Bounds3& b2)
        {
            bool x = (b1.pMax.x >= b2.pMin.x) && (b1.pMin.x <= b2.pMax.x);
            bool y = (b1.pMax.y >= b2.pMin.y) && (b1.pMin.y <= b2.pMax.y);
            bool z = (b1.pMax.z >= b2.pMin.z) && (b1.pMin.z <= b2.pMax.z);
            return (x && y && z);
        }

        bool Inside(const Vec3f& p, const Bounds3& b)
        {
            return (p.x >= b.pMin.x && p.x <= b.pMax.x && p.y >= b.pMin.y &&
                    p.y <= b.pMax.y && p.z >= b.pMin.z && p.z <= b.pMax.z);
        }

        // inline const Vector3f& operator[](int i) const{

        // }

        inline const Vec3f& operator[](int i) const
        {
            return (i == 0) ? pMin : pMax;
        }


        inline bool IntersectP(const Ray& ray, const Vec3f& invDir, const std::array<int, 3>& dirIsNeg) const;


};

inline bool Bounds3::IntersectP(const Ray& ray, const Vec3f& invDir, const std::array<int, 3>& dirIsNeg) const
{

            //std::cout << "here" << std::endl;
            float tEnter = FLT_MIN;
            float tExit = FLT_MAX;

            //std::array<float, 3> inDir = {1.0f / , 1.0f / ray.direction.y, 1.0f / ray.direction.z};

            for (int i = 0; i < 3; ++i)
            {
                // write code test if invDir is a product of dividing by zero
                
                if (std::isinf(invDir[i])){
                    if (pMin[i] > ray.origin[i] || pMax[i] < ray.origin[i]){
                        //return false;
                        continue;
                    }
                    else{
                        continue;
                    }
                }
                float t_min = (pMin[i] - ray.origin[i]) * invDir[i];
                float t_max = (pMax[i] - ray.origin[i]) * invDir[i];
                if (dirIsNeg[i] == 0)
                    std::swap(t_min, t_max); // note: here must be ==0, because dirIsNeg is actually int(x>0)
                tEnter = std::max(t_min, tEnter);
                tExit = std::min(t_max, tExit);
                

            }
            // std::cout << "node" << std::endl;
            // std::cout << tEnter << std::endl;
            // std::cout << tExit << std::endl;

            return tEnter <= tExit && tExit >= 0;
}

inline Bounds3 Union(const Bounds3& b1, const Bounds3& b2)
{
    return Bounds3(
            Vec3f( fmin(b1.pMin.x, b2.pMin.x), fmin(b1.pMin.y, b2.pMin.y),fmin(b1.pMin.z, b2.pMin.z) ),
            Vec3f( fmax(b1.pMax.x, b2.pMax.x), fmax(b1.pMax.y, b2.pMax.y),fmax(b1.pMax.z, b2.pMax.z) )
        );
}

inline Bounds3 Union(const Bounds3& b, const Vec3f& p)
{
        return Bounds3(
            Vec3f( fmin(b.pMin.x, p.x), fmin(b.pMin.y, p.y),fmin(b.pMin.z, p.z) ),
            Vec3f( fmax(b.pMax.x, p.x), fmax(b.pMax.y, p.y),fmax(b.pMax.z, p.z) )
        );
}