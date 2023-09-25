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
#include "Scene1.hpp"
#include <chrono>
#ifdef ENABLE_SYCL
  #include "sycl/sycl.hpp"
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


  Scene scene;
  scene.addMeshObj(ModelDir, "floor.obj");
  scene.addMeshObj(ModelDir, "tallbox.obj");
  scene.addMeshObj(ModelDir, "shortbox.obj");
  scene.addMeshObj(ModelDir, "left.obj");
  scene.addMeshObj(ModelDir, "right.obj");
  scene.addMeshObj(ModelDir, "light.obj");

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







  // Set up the camera parameters
  int imageWidth = 1200;
  int imageHeight = 960;
  float fov = 40.0f; // Field of view in degrees

  
  //Camera position and look direction for the Cornell Box
  Vec3f cameraPosition(278.0f, 278.0f, -800.0f); // Example camera position
  Vec3f lookAt(278.0f, 278.0f, 0.0f); // Look at the center of the Cornell Box
  Vec3f up(0.0f, 1.0f, 0.0f); // Up direction
  std::cout << scene._objectsList.size() << std::endl;
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
    int ssp = 64;

  auto startTime = std::chrono::high_resolution_clock::now();
  // Render the image

#ifdef ENABLE_SYCL

sycl::queue myQueue;  
std::cout << "Running on " << myQueue.get_device().get_info<sycl::info::device::name>() << std::endl;
std::vector<int> image(imageWidth * imageHeight * 3);
sycl::buffer<int, 1> imagebuf(image.data(), sycl::range<1>(image.size()));
myQueue.submit([&](sycl::handler& cgh) {
  sycl::stream out(1024, 256, cgh);

  auto imageAcc = imagebuf.get_access<sycl::access::mode::write>(cgh);
  cgh.parallel_for(sycl::range<2>(imageWidth, imageHeight), [=](sycl::id<2> index) {
    int i = index[0];
    int j = index[1];
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

      //std::cout << "progress : " << (float)(i + j * imageWidth) / (float)(imageWidth * imageHeight) * 100 << "%\r" << std::flush;

      auto r = compoentToint(pixelColor.x);
      auto g = compoentToint(pixelColor.y);
      auto b = compoentToint(pixelColor.z);

      int rindex = i + j * imageWidth;
      int gindex = i + j * imageWidth + 1;
      int bindex = i + j * imageWidth + 2;

      imageAcc[rindex] = r;
      imageAcc[gindex] = g;
      imageAcc[bindex] = b;

      //file << r << " " << g << " " << b << " "; 
  });
});
myQueue.wait();
myQueue.update_host(imagebuf.get_access());


for (int i = 0; i < imageWidth; ++i) 
{
    for (int j = 0; j < imageHeight; ++j) 
    {

      int rindex = i + j * imageWidth;
      int gindex = i + j * imageWidth + 1;
      int bindex = i + j * imageWidth + 2;

      auto r = image[rindex];
      auto g = image[gindex];
      auto b = image[bindex];
      file << r << " " << g << " " << b << " "; 
    }
}



#else
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

          std::cout << "progress : " << (float)(i + j * imageWidth) / (float)(imageWidth * imageHeight) * 100 << "%\r" << std::flush;

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


