#ifndef SIMPLE_IMAGE_LIBRARY_HPP
#define SIMPLE_IMAGE_LIBRARY_HPP

#include <string>
#include <vector>

enum ScalingMode {minmax, truncate};

class Image{
public:
	// Image constructors
	Image();
	Image(
		uint32_t width,
  		uint32_t height,
  		unsigned char* data,
  		uint32_t channels,
  		uint32_t bpc
	);

	Image(
		Image* image
	);

	virtual ~Image();

	// File related
	bool openFile();

	// Get attributes
	unsigned long getWidth();
	unsigned long getHeight();
	unsigned long getChannels();
	unsigned long getBpc();
	unsigned long getImageSize();
	// int getPhotometricInt();
	Image* clone();
	char* getFilename();

	// Image data related
	unsigned char* getImageData();
	unsigned char* getPixel(uint32_t x, uint32_t y);
	void setPixel(uint32_t x, uint32_t y, u_char* pixel);
	std::vector<unsigned int> getHistogram();
	std::vector<float> getCDF();
	uint32_t pixelAmount();

	// get/set Pixel methods?

protected:
	unsigned long _width{0};
	unsigned long _height{0};
	unsigned long _channels{0};
	unsigned long _bpc{0};
	// int _photometricInterpretation{-1};

	std::string _filename{};
	
	unsigned char* _data{nullptr};
};

class FloatImage : public Image {
	public:
		FloatImage(
			uint32_t width,
			uint32_t height,
			float* data,
			uint32_t channels,
			uint32_t bpc
		);

		FloatImage(FloatImage* floatImage);
		float* getImageData();
		float* getPixel(uint32_t x, uint32_t y);
		void setPixel(uint32_t x, uint32_t y, float* pixel);
		FloatImage* clone();

	private:
		float* _floatData;
};
#endif
