#include<iomanip>
#include<iostream>

#include "tiffloader.hpp"
#include "application.cpp"

using namespace std;

int main(int argc, char *argv[]) {

  if (argc == 1) {
    cout<<"Image name not provided!"<<endl;
    
    return 0;
  }

  string fileName = string(argv[1]);

  TiffLoader* tiffLoader = new TiffLoader("/home/mantas/studies/image_processing/" + fileName);
  Image* image = tiffLoader->loadImage();

  if(image == nullptr) {
    cout<<"No image loaded"<<endl;

    return 0;
  }

  cout<<"Image successfully loaded"<<endl;

  Application* application;

  application->run(tiffLoader);
}

