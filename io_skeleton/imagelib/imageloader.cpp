#include "imageloader.hpp"

#include <iostream>

ImageLoader::ImageLoader(std::string filename) : _filename{filename}{};

ImageLoader::~ImageLoader() {};

bool ImageLoader::getFileExt(std::string &fileExt){

	size_t fileExtPos = _filename.find_last_of('.');
	if (fileExtPos == std::string::npos) {
		std::cout<<"!getFileExt: "<<_filename<<std::endl;
		return false;
	}
	
	fileExt = _filename.substr(fileExtPos);
	std::cout<<"Filename extension: "<<fileExt<<std::endl;
	return true;
};

unsigned long ImageLoader::getWidth() {return _width;};
unsigned long ImageLoader::getHeight() { return _height;};
unsigned long ImageLoader::getChannels() { return _channels; };
unsigned long ImageLoader::getBPC() { return _bpc;};

