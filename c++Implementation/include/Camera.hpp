#pragma once

#include "Vec.hpp"
#include "common.hpp"


class Camera {
public:
    Camera(int width, int height, float fov, const Vec3f& position, const Vec3f& lookAt, const Vec3f& up)
        : width(width), height(height), fov(fov), position(position), lookAt(lookAt), up(up) {
        updateBasis();
    }

    void setLookAt(const Vec3f& newLookAt) {
        lookAt = newLookAt;
        updateBasis();
    }

    void setBasis(const Vec3f& newRight, const Vec3f& newUp, const Vec3f& newForward) {
        right = newRight;
        up = newUp;
        forward = newForward;
    }

    Vec3f getPosition() const {
        return position;
    }

    Vec3f getRayDirection(float x, float y, RNG &rng) const {
        float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        float halfFovTan = std::tan(Radians(fov) * 0.5f);
        float randomX = get_random_float(rng);
        float randomY = get_random_float(rng);

        //float viewX = (2.0f * (x + 0.5f) / width - 1.0f) * aspectRatio * halfFovTan;
        //float viewY = (2.0f * (y + 0.5f) / height - 1.0f) * halfFovTan;
        float viewX = (2.0f * (x + randomX) / width - 1.0f) * aspectRatio * halfFovTan;
        float viewY = (1.0f- 2.0f * (y + randomY) / height ) * halfFovTan;

        Vec3f rayDir = (viewX * right + viewY * up + forward).normalized();
        return rayDir;
    }

    Camera(const Camera& other){
        width = other.width;
        height = other.height;
        fov = other.fov;
        position = other.position;
        lookAt = other.lookAt;
        up = other.up;
        right = other.right;
        forward = other.forward;
    }




private:
    int width, height;
    float fov;
    Vec3f position, lookAt, up, right, forward;

    void updateBasis() {
        forward = (lookAt - position).normalized();
        right = crossProduct(forward, up).normalized();
        up = crossProduct(right, forward).normalized();
    }
};
