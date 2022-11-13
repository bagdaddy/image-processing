#include "image.hpp"
#include <stdexcept>
#include <iostream>
#include <vector>
#include<algorithm>

Image::Image() {

};

Image::Image(
  uint32_t width,
  uint32_t height,
  unsigned char* data,
  uint32_t channels,
  uint32_t bpc
){
  _width = width;
  _height = height;
  _data = data;
  _channels = channels;
  _bpc = bpc;
};

Image::Image(Image* image){
  _width = image->getWidth();
  _height = image->getHeight();
  _channels = image->getChannels();
  _bpc = image->getBpc();
  u_char* oldData = image->getImageData();

};

Image::~Image() {
  delete[] _data;
};

unsigned char* Image::getImageData(){
  return _data;
};

bool Image::openFile() { return false; };

unsigned char* Image::getPixel(uint32_t x, uint32_t y) {
  u_char* pixel = (u_char*) malloc(sizeof(u_char*) * _channels);
  uint32_t index = (x * this->getWidth()) + y;

  for (int i = 0; i < _channels; i++) {
    pixel[i] = _data[index + i];
  }

  return pixel;
}

void Image::setPixel(uint32_t x, uint32_t y, u_char* pixel) {
  uint32_t index = (x * this->getWidth()) + y;

  for (int i = 0; i < _channels; i++) {
    _data[index + i] = pixel[i];
  }
}

std::vector<unsigned int> Image::getHistogram(){
  if (_channels > 1) {
    throw std::invalid_argument("Histograms are only supported for grayscale images");
  }

  std::vector<unsigned int> histogramData(256, 0);

  for (int i = 0; i < _width * _height * _channels; i++) {
    uint16_t pixelVal = (uint16_t) _data[i];
    histogramData[pixelVal]++;
  }

	return histogramData;
}

std::vector<float> Image::getCDF() {
  std::vector<float> h;
  std::vector<uint32_t> h1(256, 0);;
  uint32_t MN = this->getWidth() * this->getHeight();

  for (int i = 0; i < _width * _height * _channels; i++) {
    uint16_t pixelVal = (uint16_t) _data[i];
    for(int j = pixelVal; j < 256; j++) {
      h1[j]++;
    }
  }

  for (uint32_t count : h1) {
    float res = (count * 1.0) / MN;
    h.push_back(res);
  }

  return h;
};

unsigned long Image::getImageSize() {
    return _width * _height * _channels * _bpc;
}

unsigned long Image::getWidth() {return _width;};
unsigned long Image::getHeight() { return _height;};
unsigned long Image::getChannels() { return _channels; };
unsigned long Image::getBpc() { return _bpc;};
Image* Image::clone() { 
  uint32_t size = this->pixelAmount();
  u_char* newData = (u_char*) malloc(sizeof(u_char) * size);

  for (int i = 0; i < size; i++) {
    newData[i] = _data[i];
  }
  return new Image(
    this->getWidth(),
    this->getHeight(),
    newData,
    this->getChannels(),
    this->getBpc()
  );
}

FloatImage::FloatImage(
  uint32_t width,
  uint32_t height,
  float* data,
  uint32_t channels,
  uint32_t bpc
) {
  _width = width;
  _height = height;
  _floatData = data;
  _channels = channels;
  _bpc = bpc;
}

float* FloatImage::getImageData() {
  return this->_floatData;
};

float* FloatImage::getPixel(uint32_t x, uint32_t y) {
  float* pixel = (float*) malloc(sizeof(float*) * _channels);
  uint32_t index = (x * this->getWidth()) + y;

  for (int i = 0; i < _channels; i++) {
    pixel[i] = _floatData[index + i];
  }

  return pixel;
}

void FloatImage::setPixel(uint32_t x, uint32_t y, float* pixel) {
  uint32_t index = (x * this->getWidth()) + y;

  for (int i = 0; i < _channels; i++) {
    _floatData[index + i] = pixel[i];
  }
};

FloatImage::FloatImage(FloatImage* image) {
  _width = image->getWidth();
  _height = image->getHeight();
  _floatData = image->getImageData();
  _channels = image->getChannels();
  _bpc = image->getBpc();
};

FloatImage* FloatImage::clone() { return new FloatImage(*this); };
uint32_t Image::pixelAmount() {
  return _height * _width * _channels;
}

// int Image::getPhotometricInt() { return _photometricInterpretation; };
// void Image::setPhotometricInt(int photoFamily) {
//   _photometricInterpretation = photoFamily;
// };