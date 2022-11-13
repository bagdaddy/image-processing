#ifndef SIL_TIFF_LOADER_HPP
#define SIL_TIFF_LOADER_HPP

#include "imageloader.hpp"

#include <tiffio.h> // Note use of libtiff

#include <string>

class TiffLoader : public ImageLoader {
public:
	TiffLoader(std::string filename);
	virtual ~TiffLoader();

	// Image data related
	Image* loadImage();
	void printMetaData();
	void changeDirectory(uint16_t directoryId);

	// Add getters ?
	Image* getImage();
	bool isTiled();
	bool isStriped();
protected:
	// File related
	bool openFile();
	TIFF *_tiff{nullptr};

	// functions for loading different tiff layouts
	unsigned char* loadTiles();
	unsigned char* loadStrips();
	unsigned char* loadScanline();
	
	
	// Hold Tiff metadata here?
	Image* _image{nullptr};
	unsigned int _directory{0};
	unsigned int _dirCount{0};
	bool _isTiled{false};
	bool _isStriped{false};
	// ... other?
};
#endif
