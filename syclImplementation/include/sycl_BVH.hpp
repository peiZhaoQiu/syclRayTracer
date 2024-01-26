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
#include <sycl/sycl.hpp>



struct BVHNode
{

    Bounds3 bounds = Bounds3();
    BVHNode* left = nullptr;
    BVHNode* right = nullptr;
    Object* object = nullptr;
    sycl::queue myQueue;
    int splitAxis=0, firstPrimOffset=0, nPrimitives=0;
    float area;

    BVHNode(sycl::queue queue) {
        bounds = Bounds3();
        left = nullptr;
        right = nullptr;
        object = nullptr;
        myQueue = queue;
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

    void BVHNodeDelete(BVHNode* node)
    {
        if (node->left != nullptr)
        {
            BVHNodeDelete(node->left);
        }
        if (node->right != nullptr)
        {
            BVHNodeDelete(node->right);
        }

        free(node, myQueue);
    }    

    ~BVHNode()
    {
        BVHNodeDelete(this);
    }


};




bool testIntersection(const BVHNode* node, const Ray& ray)
{

    Vec3f indiv(1.0f / ray.direction.x, 1.0f / ray.direction.y, 1.0f / ray.direction.z);
    std::array<int, 3> dirIsNeg;
    dirIsNeg[0] = ray.direction.x >0; 
    dirIsNeg[1] = ray.direction.y >0; 
    dirIsNeg[2] = ray.direction.z >0; 

    return node->bounds.IntersectP(ray, indiv, dirIsNeg);
}


class BVHAccel
{
    public:
        //enum class SplitMethod { NAIVE, SAH };
        //BVHAccel(std::vector<Object*> p, int maxPrimsInNode, SplitMethod splitMethod = SplitMethod::NAIVE);
        BVHAccel(Object** objects, int numObjects, sycl::queue queue);
        Bounds3 WorldBounds() const;
        ~BVHAccel();
        size_t _numObjects;

        Intersection Intersect(const Ray& ray) const;
        Intersection getIntersection(const BVHNode* node, const Ray& ray) const;
        void reclusiveDelete(BVHNode* node);
        BVHNode* root;

        BVHNode* build(Object** objects, int left, int right, sycl::queue queue);


        // const SplitMethod splitMethod;

        BVHAccel(const BVHAccel& bvh)
        {
            std::cout << "copy constructor called" << std::endl;
            root = bvh.root;
            _numObjects = bvh._numObjects;
        }

        BVHAccel& operator=(const BVHAccel& bvh)
        {
            std::cout << "copy constructor called" << std::endl;
            root = bvh.root;
            _numObjects = bvh._numObjects;
            return *this;
        }

        BVHAccel(BVHAccel&& bvh)
        {
            std::cout << "move constructor called" << std::endl;
            root = bvh.root;
            _numObjects = bvh._numObjects;
        }

};

BVHAccel::BVHAccel(Object** objects, int numObjects, sycl::queue queue): _numObjects(numObjects)
{
    std::cout << "BVHAccel constructor called" << std::endl;
    root = build(objects, 0, numObjects-1, queue);
}

BVHNode* BVHAccel::build(Object** objects, int left, int right, sycl::queue queue)
{
    BVHNode* node = sycl::malloc_shared<BVHNode>(1, queue);

    //std::cout << "build called: " << left << " " << right << std::endl;

    if (left == right){

        
        node->bounds = objects[left]->getBounds();
        node->object = objects[left];
        node->left = nullptr;
        node->right = nullptr;
        return node;
        //return new BVHNode(objects[left]->getBounds(), objects[left]);
    }

    if(left+1 == right)
    {
        node->left = build(objects, left, left, queue);
        node->right = build(objects, right, right, queue);
        node->bounds = Union(node->left->bounds, node->right->bounds);
        node->area = node->left->area + node->right->area;
        node->object = nullptr;
        return node;
        //return new BVHNode(Union(objects[left]->getBounds(), objects[right]->getBounds()), objects[left], objects[right]);
    }
    else
    {
        Bounds3 centroidBounds;
        for(int i = left; i<=right; i++)
        {
            centroidBounds = Union(centroidBounds, objects[i]->getBounds().Centroid());
        }
        int dim = centroidBounds.maxExtent();
        if(dim==0)
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
        node->left = build(objects, left, mid, queue);
        node->right = build(objects, mid+1, right, queue);
        node->object = nullptr;
        node->bounds = Union(node->left->bounds, node->right->bounds);
        node->area = node->left->area + node->right->area;
    }

    return node;
}

BVHAccel::~BVHAccel()
{
    if (root != nullptr)
    {
        delete root;
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
    inter._distance = INFINITY;
    inter._hit = false;
    
    
    BVHNode* stack[64] = {nullptr};
    BVHNode* curNode = (BVHNode*)node;
    int stackCount = 0;

    while((stackCount >= 0 && stackCount < 64) || curNode!=nullptr)
    {
        while(curNode != nullptr)
        {
            if(!testIntersection(curNode, ray))
            {
                curNode = nullptr;
                break;
            }
            if (curNode->object != nullptr)
            {
                Intersection tmp = curNode->object->getIntersection(ray);
                if (tmp._hit && inter._distance > tmp._distance)
                {
                    inter = tmp;
                }
                curNode = nullptr;
                break;
            }

            if(curNode->right != nullptr)
            {
                stackCount++;
                stack[stackCount] = curNode->right;
            }

            curNode = curNode->left;

        }

        if(stackCount >= 0 && stackCount < 64)
        {
            curNode = stack[stackCount];
            stackCount--;
        }
    }

    return inter;



}


