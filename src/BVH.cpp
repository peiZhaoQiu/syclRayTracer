// #include <algorithm>
// #include "BVH.hpp"
// #include <queue>



// BVHAccel::BVHAccel(Object** objects, int numObjects, SplitMethod splitMethod): _numObjects(numObjects), splitMethod(splitMethod)
// {
//     if (numObjects == 0) return;
//     root = build(objects, 0, numObjects - 1);
// }



// BVHAccel::~BVHAccel()
// {
//     if (root != nullptr)
//     {
//         reclusiveDelete(root);
//     }
    
// }


// void BVHAccel::reclusiveDelete(BVHNode* node)
// {
//     if(node->left != nullptr)  reclusiveDelete(node->left);
//     if(node->right != nullptr) reclusiveDelete(node->right);

//     if (node != nullptr){
//         if (node->object != nullptr){
//             delete node->object;
//         }
//         delete node;
//     }

    
// }




// Intersection BVHAccel::Intersect(const Ray& ray) const
// {
//     Intersection inter;
//     if (!this->root) return inter;
//     inter = getIntersection(this->root, ray);
//     return inter;
// }




// Intersection BVHAccel::getIntersection(const BVHNode* node, const Ray& ray) const
// {
//     Intersection inter;

//     Vec3f indiv(1.0f / ray.direction.x, 1.0f / ray.direction.y, 1.0f / ray.direction.z);
//     std::array<int, 3> dirIsNeg;
//     dirIsNeg[0] = ray.direction.x >0; 
//     dirIsNeg[1] = ray.direction.y >0; 
//     dirIsNeg[2] = ray.direction.z >0; 

//     if(!node->bounds.IntersectP(ray, indiv, dirIsNeg)){return inter;}

//     if (node->object != nullptr)
//     {
//         Intersection tmp = node->object->getIntersection(ray);
//         if (tmp._hit && inter._distance > tmp._distance)
//         {
//             inter = tmp;
//         }
//     }
//     else
//     {
//         if (node->left)
//         {
//             Intersection tmp = getIntersection(node->left, ray);
//             if (tmp._hit && inter._distance > tmp._distance)
//             {
//                 inter = tmp;
//             }
//         }

//         if (node->right)
//         {
//             Intersection tmp = getIntersection(node->right, ray);
//             if (tmp._hit && inter._distance > tmp._distance)
//             {
//                 inter = tmp;
//             }
//         }
//     }
//     return inter;


// }


// BVHNode* BVHAccel::build(Object** objects, int left, int right)
// {
//     BVHNode* node = new BVHNode();
//     if(left > right){return node;}

//     if(left == right)
//     {
//         node->object = objects[left];
//         node->bounds = objects[left]->getBounds();
//         node->area = objects[left]->getArea(); 
//         node->left = nullptr;
//         node->right = nullptr;
//         return node; 
//     }
//     else if (left + 1 == right)
//     {
//         node-> left = build(objects, left, left);
//         node-> right = build(objects, right, right);
//         node->bounds = Union(node->left->bounds, node->right->bounds);
//         node->area = node->left->area + node->right->area;
//         node->object = nullptr;
//         return node;
//     }
//     else
//     {
//         Bounds3 centroidBounds;
//         for (int i = left; i <= right; i++)
//         {
//             centroidBounds = Union(centroidBounds, objects[i]->getBounds().Centroid());
//         }
//         int dim = centroidBounds.maxExtent();
//         if(dim == 0)
//         {
//             std::sort(objects + left, objects + right + 1, [](Object* a, Object* b) {
//                 return a->getBounds().Centroid().x < b->getBounds().Centroid().x;
//             });
//         }
//         else if(dim == 1)
//         {
//             std::sort(objects + left, objects + right + 1, [](Object* a, Object* b) {
//                 return a->getBounds().Centroid().y < b->getBounds().Centroid().y;
//             });
//         }
//         else if(dim == 2)
//         {
//             std::sort(objects + left, objects + right + 1, [](Object* a, Object* b) {
//                 return a->getBounds().Centroid().z < b->getBounds().Centroid().z;
//             });
//         }
//         int mid = (left + right) / 2;
//         node->left = (build(objects, left, mid));
//         node->right = (build(objects, mid + 1, right)); 
//         node->object = nullptr;
//         //node->bounds = Union(node->left->bounds, node->right->bounds);
//         node->bounds = centroidBounds;
//         node->area = node->left->area + node->right->area;
//     }

//     return node;

// }

