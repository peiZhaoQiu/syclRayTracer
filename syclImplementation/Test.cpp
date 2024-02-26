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
#include <sycl/sycl.hpp>

#include "syclScene.hpp" 

template<>
struct sycl::is_device_copyable<syclScene> : std::true_type {};  
template<>
struct sycl::is_device_copyable<BVHAccel> : std::true_type {};    
template<>
struct sycl::is_device_copyable<Camera> : std::true_type {};




int compoentToint(float x){
return (int)(255*(std::pow(clamp(x, 0.0, 1.0), 1/2.2)));
//return (int)(x);  
}


int main(){

  std::string file_path = __FILE__;
  std::string dir_path = file_path.substr(0, file_path.rfind("/"));
  dir_path = dir_path.substr(0, dir_path.rfind("/"));
  std::cout<<dir_path<<std::endl;
  std::string ModelDir = dir_path + "/Model/";

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




sycl_OBJ_Loader loader;
loader.addTriangleObjectFile(ModelDir, "2.obj");
std::cout << "hello from GPGPU\n" <<std::endl;
sycl::queue myQueue(sycl::cpu_selector_v);
auto sceneObject = loader.outputSyclObj(myQueue);
syclScene scene(sceneObject->objectsList, sceneObject->materialList, sceneObject->geometryList, sceneObject->objectsListSize, sceneObject->materialListSize, sceneObject->geometryListSize, myQueue);
scene.commit();
sycl::buffer<syclScene, 1> scenebuf(&scene, sycl::range<1>(1));


std::cout << "Running on " << myQueue.get_device().get_info<sycl::info::device::name>() << std::endl;

std::vector<Vec3f> image(imageWidth * imageHeight);
sycl::buffer<Vec3f, 1> imagebuf(image.data(), sycl::range<1>(image.size()));
sycl::buffer<Camera, 1> camerabuf(&camera, sycl::range<1>(1));

myQueue.wait_and_throw();


std::cout << "submitting kernel\n";

myQueue.submit([&](sycl::handler& cgh) {
  //sycl::stream out(imageWidth, imageHeight, cgh);  
sycl::stream out(1024, 256, cgh);
auto sceneAcc = scenebuf.template get_access<sycl::access::mode::read>(cgh);
auto imageAcc = imagebuf.template get_access<sycl::access::mode::write>(cgh);
auto cameraAcc = camerabuf.template get_access<sycl::access::mode::read>(cgh);

cgh.parallel_for(sycl::range<2>(imageWidth, imageHeight), [=](sycl::id<2> index) 
{
  int i = index[0];
  int j = index[1];
  Vec3f pixelColor(0.0f, 0.0f, 0.0f);
  RNG rng(seed + i + j * imageWidth);

  for (int s = 0; s < ssp; ++s) 
  {
      //out << "progress : " << (float)(i + j * imageWidth) / (float)(imageWidth * imageHeight - 1) * 100 << "%\r" << sycl::endl;
    Vec3f rayDir = cameraAcc[0].getRayDirection(i, j, rng); 
    Ray ray(cameraAcc[0].getPosition(), rayDir); 
    auto tem = sceneAcc[0].doRendering(ray, rng);
    //out << tem.x << " " << tem.y <<" " << tem.z << sycl::endl;
    pixelColor = pixelColor + tem;
  }

  imageAcc[i + j * imageWidth] = pixelColor/ssp;

  });
});
myQueue.wait_and_throw();
std::cout << "done\n";
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





file.close();

auto endTime = std::chrono::high_resolution_clock::now();
std::chrono::duration<double> executionTime = endTime - startTime;
std::cout << "Rendering time = " << std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count() << "s" << std::endl;
std::cout << "Wrote image file " << filename << std::endl;


return 0;
}


