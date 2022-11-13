#ifndef SIL_IMAGE_LOADER_HPP
#define SIL_IMAGE_LOADER_HPP

#include "image.hpp"

#include <string>

class ImageLoader{
public:
	ImageLoader(std::string filename);
	virtual ~ImageLoader();

	// Get attributes
	unsigned long getWidth();
	unsigned long getHeight();
	unsigned long getChannels();
	unsigned long getBPC();

	// Image data related
	virtual Image* loadImage()=0;
	virtual void printMetaData()=0;
protected:
	// File related
	bool getFileExt(std::string &fileExt);

	unsigned long _width{0};
	unsigned long _height{0};
	unsigned long _channels{0};
	unsigned long _bpc{0};

	std::string _filename{};
};
#endif
