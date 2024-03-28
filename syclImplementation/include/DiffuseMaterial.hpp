
#pragma once
#include "Material.hpp"


class diffuseMaterial: public Material{
    public:


        diffuseMaterial(): Material() {
            
            _type = DIFFUSE;
        }

        diffuseMaterial(Vec3f emission, Vec3f specular, Vec3f diffuse): Material(emission, specular, diffuse) {
            _type = DIFFUSE;
        }

        Vec3f sample_virtual(const Vec3f &wi, const Vec3f &N, RNG &rng) {
            // uniform sample on the hemisphere
            float x_1 = get_random_float(rng), x_2 = get_random_float(rng);
            float z = std::fabs(1.0f - 2.0f * x_1);
            float r = std::sqrt(1.0f - z * z), phi = 2 * M_PI * x_2;
            Vec3f localRay(r*std::cos(phi), r*std::sin(phi), z);
            return toWorld(localRay, N);
        }

        Vec3f eval_virtual(const Vec3f &wi, const Vec3f &wo, const Vec3f &N){

            float cosTheta = dotProduct(N, wo);
            if (cosTheta <= 0.0f){
                return Vec3f(0.0f, 0.0f, 0.0f);
            }
            return _diffuse / M_PI;
        }

        float pdf_virtual(const Vec3f &wi, const Vec3f &wo, const Vec3f &N){
            return 0.5/M_PI;
        }
};
