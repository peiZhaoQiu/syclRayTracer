#pragma once

#include <vector>
#include <memory>
#include <ctime>
#include <ostream>
#include "Object.hpp"
#include "Bounds3.hpp"
#include "Ray.hpp"
#include "Vec.hpp"
#include "algorithm"
#include "string"


struct BVHNode
{

    Bounds3 bounds = Bounds3();
    BVHNode* left = nullptr;
    BVHNode* right = nullptr;
    Object* object = nullptr;
    int splitAxis=0, firstPrimOffset=0, nPrimitives=0;
    float area;

    BVHNode() {
        bounds = Bounds3();
        left = nullptr;
        right = nullptr;
        object = nullptr;
    }

    BVHNode(Bounds3 b, Object* o): bounds(b), object(o) {}
    BVHNode(Bounds3 b, BVHNode* l, BVHNode* r): bounds(b), left(l), right(r) {}

    std::string toString() const{
        std::string curString;
        curString = "(";
        if (object != nullptr){
            curString += " Object ";
            std::string pMinstring = std::to_string(bounds.pMin.x) + " , " + std::to_string(bounds.pMin.y) + " , " + std::to_string(bounds.pMin.z);
            std::string pMaxstring = std::to_string(bounds.pMax.x) +  ", " + std::to_string(bounds.pMax.y) + " , " + std::to_string(bounds.pMax.z);
            curString = curString + pMinstring + " , " + pMaxstring;
        }
        else{
            curString += " Node";
            if(left != nullptr){
                curString += " Left: " + left->toString();
            }
            if(right != nullptr){
                curString += " Right: " + right->toString();
            }
        }
        curString += ")";
        return curString;
    }
    friend std::ostream & operator << (std::ostream &os, const BVHNode &v)
    {
        return os << v.toString(); 
    }
};


class BVHAccel
{
    public:
        enum class SplitMethod { NAIVE, SAH };
        //BVHAccel(std::vector<Object*> p, int maxPrimsInNode, SplitMethod splitMethod = SplitMethod::NAIVE);
        BVHAccel(Object** objects, int numObjects, SplitMethod splitMethod = SplitMethod::NAIVE);
        Bounds3 WorldBounds() const;
        ~BVHAccel();
        size_t _numObjects;

        Intersection Intersect(const Ray& ray) const;
        Intersection getIntersection(const BVHNode* node, const Ray& ray) const;
        //bool IntersectP(const Ray& ray) const;
        void reclusiveDelete(BVHNode* node);
        BVHNode* root;

        BVHNode* build(Object** objects, int left, int right);


        //const int maxPrimsInNode;
        const SplitMethod splitMethod;
        //std::vector<Object*> primitives;

        //void getSample(BVHNode* node, float p, Intersection &pos, float &pdf);
        //void sample(Intersection &pos, float &pdf);
};


BVHAccel::BVHAccel(Object** objects, int numObjects, SplitMethod splitMethod): _numObjects(numObjects), splitMethod(splitMethod)
{
    if (numObjects == 0) return;
    root = build(objects, 0, numObjects - 1);
}



BVHAccel::~BVHAccel()
{
    if (root != nullptr)
    {
        reclusiveDelete(root);
    }
    
}


void BVHAccel::reclusiveDelete(BVHNode* node)
{
    if(node->left != nullptr)  reclusiveDelete(node->left);
    if(node->right != nullptr) reclusiveDelete(node->right);

    if (node != nullptr){
        if (node->object != nullptr){
            delete node->object;
        }
        delete node;
    }   
}

Intersection BVHAccel::Intersect(const Ray& ray) const
{
    Intersection inter;
    if (!this->root) return inter;
    inter = getIntersection(this->root, ray);
    return inter;
}


Intersection BVHAccel::getIntersection(const BVHNode* node, const Ray& ray) const
{
    Intersection inter;
    
    Vec3f indiv(1.0f / ray.direction.x, 1.0f / ray.direction.y, 1.0f / ray.direction.z);
    std::array<int, 3> dirIsNeg;
    dirIsNeg[0] = ray.direction.x >0; 
    dirIsNeg[1] = ray.direction.y >0; 
    dirIsNeg[2] = ray.direction.z >0; 

    if(!node->bounds.IntersectP(ray, indiv, dirIsNeg)){return inter;}
    if (node->object != nullptr)
    {
       
        Intersection tmp = node->object->getIntersection(ray);
        if (tmp._hit && inter._distance > tmp._distance)
        {
            inter = tmp;
        }
    }
    else
    {
        if (node->left)
        {
            Intersection tmp = getIntersection(node->left, ray);
            if (tmp._hit && inter._distance > tmp._distance)
            {
                std::cout << "here" << std::endl;
                inter = tmp;
            }
        }

        if (node->right)
        {
            Intersection tmp = getIntersection(node->right, ray);
            if (tmp._hit && inter._distance > tmp._distance)
            {
                std::cout << "here" << std::endl;
                inter = tmp;
            }
        }
    }
    return inter;


}


BVHNode* BVHAccel::build(Object** objects, int left, int right)
{
    //std::cout << left << "~~~~~~" << right <<std::endl;
    BVHNode* node = new BVHNode();
    if(left > right){return node;}

    if(left == right)
    {
        node->object = objects[left];
        node->bounds = objects[left]->getBounds();
        node->area = objects[left]->getArea(); 
        node->left = nullptr;
        node->right = nullptr;
        return node; 
    }
    else if (left + 1 == right)
    {
        node-> left = build(objects, left, left);
        node-> right = build(objects, right, right);
        node->bounds = Union(node->left->bounds, node->right->bounds);
        node->area = node->left->area + node->right->area;
        node->object = nullptr;
        return node;
    }
    else
    {
        Bounds3 centroidBounds;
        for (int i = left; i <= right; i++)
        {
            centroidBounds = Union(centroidBounds, objects[i]->getBounds().Centroid());
        }
        int dim = centroidBounds.maxExtent();
        if(dim == 0)
        {
            std::sort(objects + left, objects + right + 1, [](Object* a, Object* b) {
                return a->getBounds().Centroid().x < b->getBounds().Centroid().x;
            });
        }
        else if(dim == 1)
        {
            std::sort(objects + left, objects + right + 1, [](Object* a, Object* b) {
                return a->getBounds().Centroid().y < b->getBounds().Centroid().y;
            });
        }
        else if(dim == 2)
        {
            std::sort(objects + left, objects + right + 1, [](Object* a, Object* b) {
                return a->getBounds().Centroid().z < b->getBounds().Centroid().z;
            });
        }
        int mid = (left + right) / 2;
        node->left = (build(objects, left, mid));
        node->right = (build(objects, mid + 1, right)); 
        node->object = nullptr;
        //node->bounds = Union(node->left->bounds, node->right->bounds);
        node->bounds = centroidBounds;
        node->area = node->left->area + node->right->area;
    }

    return node;

}