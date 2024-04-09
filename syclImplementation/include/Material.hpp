#pragma once

#include "Vec.hpp"
#include "common.hpp"



enum MaterialType {DIFFUSE, EMISSION};

struct MaterialInfo{
    Vec3f _emission;
    Vec3f _specular;
    Vec3f _diffuse;
    MaterialInfo(Vec3f emission, Vec3f specular, Vec3f diffuse): _emission(emission), _specular(specular), _diffuse(diffuse) {}
};


class Material
{

    public:

        Material() {
        }
        MaterialType _type;
        float pdf(const Vec3f &wi, const Vec3f &wo, const Vec3f &N) const;
        Vec3f sample(const Vec3f &wi, const Vec3f &N, RNG &rng) const;
        Material(Vec3f emission, Vec3f specular, Vec3f diffuse): _emission(emission), _specular(specular), _diffuse(diffuse) {} 
        Vec3f eval(const Vec3f &wi, const Vec3f &wo, const Vec3f &N)const;               

        Vec3f _emission;
        Vec3f _specular;
        Vec3f _diffuse;

        bool getEmission() const
        {
            return  _emission.length() > MyEPSILON;
        }

    
};

#include "DiffuseMaterial.hpp"

float Material::pdf(const Vec3f &wi, const Vec3f &wo, const Vec3f &N)const{
    switch (_type)
    {
    case DIFFUSE:
        return static_cast<const diffuseMaterial*>(this)->pdf_virtual(wi, wo, N);
    default:
        return 0.0f;
    }
}

Vec3f Material::sample(const Vec3f &wi, const Vec3f &N, RNG &rng)const{
    switch (_type)
    {
    case DIFFUSE:
        return static_cast<const diffuseMaterial*>(this)->sample_virtual(wi, N, rng);
    default:
        return Vec3f(0.0f, 0.0f, 0.0f);
    }
}

Vec3f Material::eval(const Vec3f &wi, const Vec3f &wo, const Vec3f &N)const{
    switch (_type)
    {
    case DIFFUSE:
        return static_cast<const diffuseMaterial*>(this)->eval_virtual(wi, wo, N);
    default:
        return Vec3f(0.0f, 0.0f, 0.0f);
    }
}




class MaterialFactory{
    public:
    static Material* createMaterial(Vec3f emission, Vec3f specular, Vec3f diffuse){
        return new diffuseMaterial(emission, specular, diffuse);
    } 
};













