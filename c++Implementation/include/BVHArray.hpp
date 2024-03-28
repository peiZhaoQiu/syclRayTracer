#pragma once

#include "ObjectList.hpp"

struct BVHNode
{
    long _rightIndex = -1;
    long _leftIndex = -1;
    long _objectIndex = -1;
    Bounds3 _bounds;
};

class BVHArray
{

    long _arraySize = 0;
    BVHNode* _array = nullptr;
    BVHArray() = delete;
    
    long caculateArraySize(int numObjects)
    {
        return 2 * numObjects - 1;
    }

    long buildTree(ObjectList* sceneObject, int left, int right);

    Intersection getIntersection(const long index, const Ray& ray, ObjectList* objects) const;
    
    bool haveNode(const long index) const
    {
        if (index >= _arraySize || index < 0){

            //std::cout << "The index is out of range" << std::endl;
            return false;
        } 

        return true;
    }   

    public:

    BVHArray(ObjectList* sceneObject);
    Intersection Intersect(const Ray& ray, ObjectList* objects) const;

    const BVHNode* getNode(long index) const
    {
        if (haveNode(index))
        {
            return &_array[index];
        }
        return nullptr;
    }

    long getArraySize() const
    {
        return _arraySize;
    }   

    ~BVHArray()
    {
        if (_array != nullptr)
        {
            delete[] _array;
        }
    }
};

bool testIntersection(const BVHNode* node, const Ray& ray)
{
    Vec3f indiv(1.0f / ray.direction.x, 1.0f / ray.direction.y, 1.0f / ray.direction.z);
    std::array<int, 3> dirIsNeg;
    dirIsNeg[0] = ray.direction.x >0; 
    dirIsNeg[1] = ray.direction.y >0; 
    dirIsNeg[2] = ray.direction.z >0; 

    return node->_bounds.IntersectP(ray, indiv, dirIsNeg);
}

BVHArray::BVHArray(ObjectList* sceneObject)
{
    if(sceneObject == nullptr) return;
    if(sceneObject->getObjectsListSize() == 0) return;
    size_t objectsListSize = sceneObject->getObjectsListSize();
    _arraySize = caculateArraySize(objectsListSize);
    std::cout << "The array size is " << _arraySize << std::endl;
    _array = new BVHNode[_arraySize];
    buildTree(sceneObject, 0, objectsListSize - 1);
}



long BVHArray::buildTree(ObjectList* sceneObject, int left, int right)
{

    static long index = 0;

    if(!haveNode(index))
    {
        std::cout << index << std::endl;
        std::cout << left << "  " << right << std::endl;
        return -1;
    }
    
    long curIndex = index;
    index = index + 1;
    
    
    if (left == right)
    {
        _array[curIndex]._objectIndex = left;
        _array[curIndex]._bounds = sceneObject->getBounds(left);
        return curIndex;
    }
    else if (left + 1 == right)
    {
        long leftIndex = buildTree(sceneObject, left, left);
        long rightIndex = buildTree(sceneObject, right, right);
        _array[curIndex]._leftIndex = leftIndex; 
        _array[curIndex]._rightIndex = rightIndex;
        
        _array[curIndex]._bounds = Union(_array[leftIndex]._bounds, _array[rightIndex]._bounds);
        return curIndex;
    }
    sceneObject->spacePartitioning(left, right);
    long mid = (left + right) / 2;
    

    long leftIndex = buildTree(sceneObject,  left, mid);
    long rightIndex = buildTree(sceneObject, mid + 1, right); 
    _array[curIndex]._leftIndex = leftIndex;
    _array[curIndex]._rightIndex = rightIndex;

    //std::cout << "The left index is " << leftIndex << "  The right index is " << rightIndex << std::endl;
    if(haveNode(leftIndex) && haveNode(rightIndex))
    {
         _array[curIndex]._bounds = Union(_array[leftIndex]._bounds, _array[rightIndex]._bounds);
    }
   
    return curIndex;
}


Intersection BVHArray::Intersect(const Ray& ray, ObjectList* objects) const
{
    Intersection inter;
    if (_array == nullptr) return inter;
    inter = getIntersection(0, ray, objects);
    return inter;
}

Intersection BVHArray::getIntersection(const long index, const Ray& ray, ObjectList* objects) const
{
    if (!haveNode(index)) return Intersection();

    Intersection inter;
    inter._hit = false;
    inter._distance = INFINITY;


    BVHNode* stack[64] = {nullptr};
    BVHNode* curNode = &(_array[index]);
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
            if (curNode->_objectIndex > 0)
            {
                auto curObjectIndex = curNode->_objectIndex;
                Intersection tmp = objects->getIntersection(ray, curObjectIndex);
                if (tmp._hit && inter._distance > tmp._distance)
                {
                    inter = tmp;
                }
                curNode = nullptr;
                break;
            }
            long curRight = curNode->_rightIndex;
            if(haveNode(curRight))
            {
                stackCount++;
                stack[stackCount] = &_array[curRight];
            }
            long curLeft = curNode->_leftIndex;
            if (haveNode(curLeft))
            {
                curNode = &_array[curLeft];
            }
            else
            {
                curNode = nullptr;
            }
        }

        if(stackCount >= 0 && stackCount < 64)
        {
            curNode = stack[stackCount];
            stackCount--;
        }
    }

    return inter;
 
}


