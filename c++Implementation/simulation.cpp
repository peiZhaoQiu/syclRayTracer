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
#include "Scene.hpp"


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



  OBJ_Loader loader;
  loader.addTriangleObjectFile(ModelDir, "cornell_box.obj");
  //loader.addTriangleObjectFile(ModelDir, "2.obj");
  auto sceneObject = loader.outputObj();
  Scene scene(sceneObject.get());
  // scene.commit();
  // std::cout << "there are " <<scene._objectsListSize << " objects in the scene"<<std::endl;

  // for (int j = 0; j < imageHeight; ++j) 
  // {
  //     for (int i = 0; i < imageWidth; ++i) 
  //     {
  //       Vec3f pixelColor(0.0f, 0.0f, 0.0f);
  //       RNG rng(seed + i + j * imageWidth);
  //       for (int s = 0; s < ssp; ++s) 
  //       {

  //         Vec3f rayDir = camera.getRayDirection(i, j, rng);
           
  //         Ray ray(camera.getPosition(), rayDir);
          
          
  //         auto tem = scene.doRendering(ray, rng);

  //         pixelColor = pixelColor + tem;
  //       }

  //         pixelColor = pixelColor/ ssp;
  //         //std::cout << "progress : " << (float)(i + j * imageWidth) / (float)(imageWidth * imageHeight - 1) * 100 << "%\r" << std::flush;

  //         auto r = compoentToint(pixelColor.x);
  //         auto g = compoentToint(pixelColor.y);
  //         auto b = compoentToint(pixelColor.z);

  //         file << r << " " << g << " " << b << " "; 
  //     }
  // }



  file.close();

  auto endTime = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> executionTime = endTime - startTime;
  std::cout << "Rendering time = " << std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count() << "s" << std::endl;
  std::cout << "Wrote image file " << filename << std::endl;


  return 0;
}


