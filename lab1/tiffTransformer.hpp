#ifndef SIL_TIFF_TRANSFORMER_HPP
#define SIL_TIFF_TRANSFORMER_HPP
#include <iostream>
#include <iomanip>
#include "image.hpp"
#include <vector>
using namespace std;

enum SpatialFilterType {correlation, convolution};

class TiffTransformer {
    public:
        Image* intensityNegation(Image* image);
        Image* intensityPowerlaw(Image* image, float gamma, uint32_t c);
        Image* thresholdSingle(Image* image, uint32_t cutoff);
        Image* thresholdRange(Image* image, uint32_t floor, uint32_t ceil);
        Image* intensityLUT(Image *image, u_char* lut);
        Image* histogramEqualization(Image *image);
        FloatImage* spatialFilter(Image* image, vector<vector<float>> filter, SpatialFilterType type);
        FloatImage* sobelGradient(Image* image, vector<vector<float>> filterX, vector<vector<float>> filterY);
        u_char* negationLUT();
        u_char* powerLawLUT(float gamma, uint32_t c);
        FloatImage* toFloat(Image* image);
        void to8bit(FloatImage* floatImg, Image* image, ScalingMode mode);
        void addImages(FloatImage* image1, FloatImage* image2);
        void addImages(Image* image1, Image* image2, float k, ScalingMode mode);
        void subtractImages(Image* image1, Image* image2);
        void scaleMinMax(uint32_t* data, uint32_t pixelAmount);
        void scaleTruncate(uint32_t* data, uint32_t pixelAmount);
        
    private:
        float* applyFilter(FloatImage* image, vector<vector<float>> filter, uint32_t x, uint32_t y, SpatialFilterType type);
        float scaleToAppropriateRange(float value);
		void scaleConvertMinMax(FloatImage* floatImg, Image* image);
        void scaleConvertTruncate(FloatImage* floatImg, Image* image);
};

#endif