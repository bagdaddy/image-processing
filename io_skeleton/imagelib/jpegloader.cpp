#include "jpegloader.hpp"

#include <iostream>

JpegLoader::JpegLoader(std::string filename) : ImageLoader(filename){
	// yes, should ideally throw an exception on fail, we skip that.
	openFile();
};

JpegLoader::~JpegLoader() {};

Image* JpegLoader::loadImage(){
	std::cout<<"JpegLoader::loadImageData()"<<std::endl;
	std::cout<<"- not implemented"<<std::endl;
	return nullptr;
}

void JpegLoader::printMetaData(){
	std::cout<<"JpegLoader::printMetaData()"<<std::endl;
	std::cout<<"- not implemented"<<std::endl;
}

bool JpegLoader::openFile(){
	std::cout<<"JpegLoader::openFile()"<<std::endl;
	std::cout<<"- not implemented"<<std::endl;
	return false;
};

// unsigned long JpegLoader::getWidth() {return _width;};
// unsigned long JpegLoader::getHeight() { return _height;};
// unsigned long JpegLoader::getChannels() { return _channels; };
// unsigned long JpegLoader::getBPP() { return _bpp;};

