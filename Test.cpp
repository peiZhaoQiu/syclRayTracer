// Copyright 2009-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include <stdio.h>
#include <math.h>
#include <limits>
#include <stdio.h>
#include <tiny_obj_loader.h>
#include <iostream>
#include "Vec.hpp"
#include "Camera.hpp" 
#include <fstream>
#include "common.hpp"

#include <chrono>
#ifdef ENABLE_SYCL
  #include <sycl/sycl.hpp>
#ifdef ENABLE_GPGPU
  #include "syclScene.hpp" 
  template<>
  struct sycl::is_device_copyable<syclScene> : std::true_type {};  
#else
  #include "Scene1.hpp"
  template<>
  struct sycl::is_device_copyable<Scene> : std::true_type {};  
#endif


  template<>
  struct sycl::is_device_copyable<Camera> : std::true_type {};

  template<>
  struct sycl::is_device_copyable<BVHAccel> : std::true_type {};

#else
  #include "Scene1.hpp"
#endif

int compoentToint(float x){
return (int)(255*(std::pow(clamp(x, 0.0, 1.0), 1/2.2)));
//return (int)(x);  
}


int main(){

  std::string file_path = __FILE__;
  //std::string file_path_1 = file_path.substr(0, file_path.rfind("/"));
  std::string dir_path = file_path.substr(0, file_path.rfind("/"));
 // std::cout<< file_path_1<< std::endl;
  std::cout<<dir_path<<std::endl;
  std::string ModelDir = dir_path + "/Model/";


  
  // scene.addMeshObj(ModelDir, "floor.obj");
  // scene.addMeshObj(ModelDir, "tallbox.obj");
  // scene.addMeshObj(ModelDir, "shortbox.obj");
  // scene.addMeshObj(ModelDir, "left.obj");
  // scene.addMeshObj(ModelDir, "right.obj");
  // scene.addMeshObj(ModelDir, "light.obj");


  //scene.addMeshObj(ModelDir, "cornell_box.obj");

  // Ray ray1(Vec3f(0.33f,0.33f,10.0f), Vec3f(0,0,1));
  // Ray ray2(Vec3f(1.00f,100.00f,1.0f), Vec3f(0,-1,0));
  // Ray ray3(Vec3f(150.00f,240.00f,167.0f), Vec3f(0,-1,0));
  // Ray ray4(Vec3f(278.0f, 278.0f, -800.0f), Vec3f(0,-1,0));
  // Ray ray5(Vec3f(1.00f,100.00f,1.0f), Vec3f(0,-1,0));
  // Ray ray6(Vec3f(150.0f, 240.0f, 167.0f), Vec3f(0,-1,0));
  // auto l = scene.castRay(ray1);
  // auto k = scene.castRay(ray2);
  // auto m = scene.castRay(ray3);
  // auto n = scene.castRay(ray4);
  // auto o = scene.castRay(ray5);
  // auto p = scene.castRay(ray6);

  //std::cout <<scene._bvh->root->toString()<<std::endl;

  // std::cout << "l: " << l._hit << std::endl;
  // std::cout << "k: " << k._hit << std::endl;
  // std::cout << "m: " << m._hit << std::endl;
  // std::cout << "n: " << n._hit << std::endl;
  // std::cout << "o: " << o._hit << std::endl;
  // std::cout << "p: " << p._hit << std::endl;







  // // Set up the camera parameters
  int imageWidth = 1200/10;
  int imageHeight = 960/10;
  float fov = 40.0f; // Field of view in degrees


  // //Camera position and look direction for the Cornell Box
  Vec3f cameraPosition(278.0f, 278.0f, -800.0f); // Example camera position
  Vec3f lookAt(278.0f, 278.0f, 0.0f); // Look at the center of the Cornell Box
  Vec3f up(0.0f, 1.0f, 0.0f); // Up direction

  unsigned int seed = 123;  


  Camera camera(imageWidth, imageHeight, fov, cameraPosition, lookAt, up);

    std::string filename = "image.ppm"; 
    std::ofstream file;
    file.open(filename, std::ios::binary); 
    file << "P3\n" << imageWidth << " " << imageHeight << "\n255\n";
    if(!file.is_open()){
        std::cout << "Error: Could not open file " << filename << std::endl;
        return -1;
    }
    int ssp = 64;//*4;

  auto startTime = std::chrono::high_resolution_clock::now();
  // Render the image

#ifdef ENABLE_SYCL

#ifdef ENABLE_GPGPU
sycl_OBJ_Loader loader;
loader.addTriangleObjectFile(ModelDir, "cornell_box.obj");
sycl::queue myQueue(sycl::gpu_selector{});
auto sceneObject = loader.outputSyclObj(myQueue);
syclScene scene(sceneObject->objectsList, sceneObject->materialList, sceneObject->geometryList, sceneObject->objectsListSize, sceneObject->materialListSize, sceneObject->geometryListSize, myQueue);
sycl::buffer<syclScene, 1> scenebuf(&scene, sycl::range<1>(1));
#else
  std::cout << "here" <<std::endl;
  sycl::queue myQueue(sycl::cpu_selector{});
  OBJ_Loader loader;
  loader.addTriangleObjectFile(ModelDir, "cornell_box.obj");
  auto sceneObject = loader.outputObj();
  Scene scene(sceneObject->objectsList, sceneObject->materialList, sceneObject->geometryList, sceneObject->objectsListSize, sceneObject->materialListSize, sceneObject->geometryListSize);
  sycl::buffer<Scene, 1> scenebuf(&scene, sycl::range<1>(1));
#endif

std::cout << "Running on " << myQueue.get_device().get_info<sycl::info::device::name>() << std::endl;

std::vector<Vec3f> image(imageWidth * imageHeight);
sycl::buffer<Vec3f, 1> imagebuf(image.data(), sycl::range<1>(image.size()));

sycl::buffer<Camera, 1> camerabuf(&camera, sycl::range<1>(1));
//std::cout << "starting rendering" << std::endl;
myQueue.submit([&](sycl::handler& cgh) {
  //sycl::stream out(imageWidth, imageHeight, cgh);  
  //sycl::stream out(1024, 256, cgh);
  //out << "starting rendering" << sycl::endl;
  auto sceneAcc = scenebuf.template get_access<sycl::access::mode::read>(cgh);
  //out << "starting rendering 1" << sycl::endl;
  auto imageAcc = imagebuf.template get_access<sycl::access::mode::write>(cgh);
  auto cameraAcc = camerabuf.template get_access<sycl::access::mode::read>(cgh);
  
  cgh.parallel_for(sycl::range<2>(imageWidth, imageHeight), [=](sycl::id<2> index) {

    
    //out << "here" << sycl::endl;
    int i = index[0];
    int j = index[1];
    Vec3f pixelColor(0.0f, 0.0f, 0.0f);
    RNG rng(seed + i + j * imageWidth);
    for (int s = 0; s < ssp; ++s) 
    {
      //out << "progress : " << (float)(i + j * imageWidth) / (float)(imageWidth * imageHeight - 1) * 100 << "%\r" << std::endl;
      Vec3f rayDir = cameraAcc[0].getRayDirection(i, j, rng);
       
      Ray ray(cameraAcc[0].getPosition(), rayDir);
      
      
      auto tem = sceneAcc[0].doRendering(ray, rng);

      pixelColor = pixelColor + tem;
    }

    pixelColor = pixelColor/ ssp;

  imageAcc[i + j * imageWidth] = pixelColor;

  });
});
myQueue.wait();
myQueue.update_host(imagebuf.get_access());
std::cout << "finished rendering" << std::endl;

for (int j = 0; j < imageHeight; ++j)
{
     for (int i = 0; i < imageWidth; ++i) 
    {


      auto currentColor = image[i + j * imageWidth];

      auto r = compoentToint(currentColor.x);
      auto g = compoentToint(currentColor.y);
      auto b = compoentToint(currentColor.z);



      file << r << " " << g << " " << b << " "; 
    }
}


#else

  OBJ_Loader loader;
  //loader.addTriangleObjectFile(ModelDir, "cornell_box.obj");
  loader.addTriangleObjectFile(ModelDir, "2.obj");
  auto sceneObject = loader.outputObj();
  Scene scene(sceneObject->objectsList, sceneObject->materialList, sceneObject->geometryList, sceneObject->objectsListSize, sceneObject->materialListSize, sceneObject->geometryListSize);
  scene.commit();
  std::cout << "there are " <<scene._objectsListSize << " objects in the scene"<<std::endl;

  for (int j = 0; j < imageHeight; ++j) 
  {
      for (int i = 0; i < imageWidth; ++i) 
      {
        Vec3f pixelColor(0.0f, 0.0f, 0.0f);
        RNG rng(seed + i + j * imageWidth);
        for (int s = 0; s < ssp; ++s) 
        {

          Vec3f rayDir = camera.getRayDirection(i, j, rng);
           
          Ray ray(camera.getPosition(), rayDir);
          
          
          auto tem = scene.doRendering(ray, rng);

          pixelColor = pixelColor + tem;
        }

          pixelColor = pixelColor/ ssp;
          //std::cout << "progress : " << (float)(i + j * imageWidth) / (float)(imageWidth * imageHeight - 1) * 100 << "%\r" << std::flush;

          auto r = compoentToint(pixelColor.x);
          auto g = compoentToint(pixelColor.y);
          auto b = compoentToint(pixelColor.z);

          file << r << " " << g << " " << b << " "; 
      }
  }

#endif


  file.close();

  auto endTime = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> executionTime = endTime - startTime;
  std::cout << "Rendering time = " << std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count() << "s" << std::endl;
  std::cout << "Wrote image file " << filename << std::endl;


  return 0;
}


