#pragma once 
#include <cmath>
#include <limits>

#include <oneapi/dpl/random>
typedef oneapi::dpl::minstd_rand RNG;
#undef M_PI
#define M_PI 3.14159265358979323846f

const float MyEPSILON = std::numeric_limits<float>::epsilon();
const float kInfinity = std::numeric_limits<float>::max();

inline float Radians(float deg) { return (M_PI / 180.f) * deg; }
inline float Degrees(float rad) { return (180.f / M_PI) * rad; }

inline float clamp(float val, float low, float high) {
    if (val < low) return low;
    else if (val > high) return high;
    else return val;
}


float get_random_float(RNG &rng)
{
    oneapi::dpl::uniform_real_distribution<float> distribution(0.f, 1.f);
    return distribution(rng);
}

inline Vec3f toWorld(const Vec3f &a, const Vec3f &N){
    Vec3f B, C;
    if (std::fabs(N.x) > std::fabs(N.y)){
        float invLen = 1.0f / std::sqrt(N.x * N.x + N.z * N.z);
        C = Vec3f(N.z * invLen, 0.0f, -N.x *invLen);
    }
    else {
        float invLen = 1.0f / std::sqrt(N.y * N.y + N.z * N.z);
        C = Vec3f(0.0f, N.z * invLen, -N.y *invLen);
    }
    B = crossProduct(C, N);
    return a.x * B + a.y * C + a.z * N;
}