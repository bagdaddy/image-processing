#include "tiffloader.hpp"
#include <iostream>
#include <string.h>

TiffLoader::TiffLoader(std::string filename) : ImageLoader(filename){
	// ideally throw exceptions on fail, we skip that
	openFile();
};

TiffLoader::~TiffLoader() {};

bool TiffLoader::openFile(){
	std::cout<<"TiffLoader::OpenFile: "<<_filename<<std::endl;
	std::string fileExt{};

	if(getFileExt(fileExt) && (fileExt.compare(".tif") == 0) || (fileExt.compare(".tiff") == 0)){

		std::cout<<"Opening tif: "<<_filename<<std::endl;
		_tiff = TIFFOpen(_filename.c_str(),"r");

		if(_tiff != nullptr){
			_dirCount = TIFFNumberOfDirectories(_tiff);
			std::cout<<"Tiff DIR count: "<<_dirCount<<std::endl;

			// import necessary meta data
			_isStriped = !TIFFIsTiled(_tiff);
			_isTiled = TIFFIsTiled(_tiff);
		}
		return true;
	};

	std::cout<<"Error loading as tif: "<<_filename<<std::endl;
	return false;
};

void TiffLoader::printMetaData(){
	std::cout<<"Filename: "<<_filename<<std::endl;
	std::cout<<"Height: "<<_image->getHeight()<<std::endl;
	std::cout<<"Width: "<<_image->getWidth()<<std::endl;
	std::cout<<"Bytes per channel: "<<_image->getBpc()<<std::endl;
	std::cout<<"Channels: "<<_image->getChannels()<<std::endl;
};

void TiffLoader::changeDirectory(uint16_t directoryId){
	TIFFSetDirectory(_tiff, directoryId);
}

Image* TiffLoader::loadImage(){
	// yes, should ideally throw an exception on fail, we skip that.
	uint32_t width;
	uint32_t height;
	uint16_t bitsPerChannel;
	uint32_t channels;
	uint32_t photometric;

	if(_tiff){
		unsigned char* imageData{nullptr};
		if(_isStriped)
			imageData =  loadStrips();
		else if(_isTiled)
			imageData = loadTiles();
		else
			imageData = loadScanline();

		if(!imageData){
			return nullptr;
		}

		// construct and return an Image!
		TIFFGetField(_tiff, TIFFTAG_IMAGEWIDTH, &width);
		TIFFGetField(_tiff, TIFFTAG_IMAGELENGTH, &height);
		TIFFGetField(_tiff, TIFFTAG_BITSPERSAMPLE, &bitsPerChannel);
		TIFFGetField(_tiff, TIFFTAG_SAMPLESPERPIXEL, &channels);
		// TIFFGetField(_tiff, TIFFTAG_PHOTOMETRIC, &photometric);

		_image = new Image(width, height, imageData, channels, bitsPerChannel);

		// _image->setPhotometricInt(photometric);

		return _image;
	}
	return nullptr;
};

unsigned char* TiffLoader::loadStrips(){
	if (_tiff == nullptr) {
		return nullptr;
	}

	uint32_t rowsPerStrip;
	u_char* buf;
	tstrip_t strip = 1;
	tmsize_t stripSize = TIFFStripSize(_tiff);
	uint32_t dataIndex;
	uint32_t numOfStrips = TIFFNumberOfStrips(_tiff), imageWidth, i, c;
	u_char* data = (u_char*) _TIFFmalloc(numOfStrips * stripSize);
	buf = (u_char*)_TIFFmalloc(stripSize);

	TIFFGetField(_tiff, TIFFTAG_SAMPLESPERPIXEL, &c);
	TIFFGetField(_tiff, TIFFTAG_ROWSPERSTRIP, &rowsPerStrip);
	TIFFGetField(_tiff, TIFFTAG_IMAGEWIDTH, &imageWidth);

	for (strip = 0; strip < numOfStrips; strip++) {
		TIFFReadEncodedStrip(_tiff, strip, buf, (tsize_t) - 1);
		for (i = 0; i < stripSize; i++) {
			dataIndex = strip * rowsPerStrip * imageWidth * c + i;
			if (i < numOfStrips * stripSize) {
				data[dataIndex] = buf[i];
			}
		}
	}

	_TIFFfree(buf);

	return data;
};

unsigned char* TiffLoader::loadTiles(){
	if (_tiff == nullptr) {
		return nullptr;
	}

	std::cout<<"loading tiled image"<<std::endl;

	tmsize_t tileSize = TIFFTileSize(_tiff);
	uint32_t numOfTiles = TIFFNumberOfTiles(_tiff);
	uint32_t imageWidth, imageLength;
	uint32_t tileWidth, tileLength, realTileLength, realTileWidth;
	uint32_t x, y, i;
	u_char* buf;
	u_char* data = (u_char*) _TIFFmalloc(numOfTiles * tileSize);
	uint32_t c; 
	uint32_t s = 0;

	TIFFGetField(_tiff, TIFFTAG_SAMPLESPERPIXEL, &c);
	TIFFGetField(_tiff, TIFFTAG_IMAGEWIDTH, &imageWidth);
	TIFFGetField(_tiff, TIFFTAG_IMAGELENGTH, &imageLength);
	TIFFGetField(_tiff, TIFFTAG_TILEWIDTH, &tileWidth);
	TIFFGetField(_tiff, TIFFTAG_TILELENGTH, &tileLength);
	realTileWidth = c * tileWidth;
	realTileLength = c * tileLength;

	buf = (u_char*)_TIFFmalloc(tileSize);
	x = 0 * tileWidth;
	y = 0 * tileLength;
	std::cout<<"img width: "<<imageWidth<<", image height: "<<imageLength<<std::endl;
	std::cout<<"tile size: "<<tileSize<<", num of tiles: "<<numOfTiles<<std::endl;
	for (y = 0; y < imageLength; y += tileLength) {
		for (x = 0; x < imageWidth; x += tileWidth) {
			TIFFReadTile(_tiff, buf, x, y, 0, 0);
			for (i = 0; i < tileSize; i++) {
				uint32_t dataIndex;

				//if at the edge of image, stop reading data from buffer until we reach the start of next line
				if (x + tileLength > imageWidth) {
					uint32_t pixelCutoff = (imageWidth - x) * c;
					if (realTileWidth > i % realTileWidth && i % realTileWidth > pixelCutoff) {
						continue;
					}
				}

				dataIndex = x * c + (y + i / (realTileWidth)) * imageWidth * c + i % (realTileWidth);

				if (dataIndex > numOfTiles * tileSize) {
					break;
				}

				data[dataIndex] = buf[i];
			}
		}
	}

	_TIFFfree(buf);
	
	return data;
};

unsigned char* TiffLoader::loadScanline(){
	std::cout<<"Loading scan lines"<<std::endl;
	if (_tiff == nullptr) {
		return nullptr;
	}
	uint32_t imagelength;
	unsigned char* buf;
	uint32_t row;

	TIFFGetField(_tiff, TIFFTAG_IMAGELENGTH, &imagelength);
	buf = (unsigned char*) _TIFFmalloc(TIFFScanlineSize(_tiff));
	for (row = 0; row < imagelength; row++)
	    TIFFReadScanline(_tiff, buf, row);

	return buf;
};

Image* TiffLoader::getImage() {
	return _image;
}

bool TiffLoader::isStriped() {
	return _isStriped;
}

bool TiffLoader::isTiled() {
	return _isTiled;
}