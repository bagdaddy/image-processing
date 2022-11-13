#ifndef SIL_JPEG_LOADER_HPP
#define SIL_JPEG_LOADER_HPP

#include "imageloader.hpp"

#include <string>

class JpegLoader : public ImageLoader {
public:
	JpegLoader(std::string filename);
	virtual ~JpegLoader();

	// Image data related
	Image* loadImage();
	void printMetaData();
protected:
	// File related
	bool openFile();;
};
#endif
