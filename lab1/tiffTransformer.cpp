#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <stdexcept>
#include <math.h>
#include <algorithm>
#include "tiffTransformer.hpp"
#include "helpers.hpp"
#include "image.hpp"
using namespace std;

//s = L-1-r
Image* TiffTransformer::intensityNegation(Image* image) {
    uint32_t maxIntensity = pow(2, image->getBpc());

    uint32_t channels = image->getChannels();
    for (int x = 0; x < image->getWidth(); x++) {
        for (int y = 0; y < image->getHeight(); y++) {

            u_char* pixel = image->getPixel(x, y);
            for (int j = 0; j < image->getChannels(); j++) {
                pixel[j] = (u_char) (maxIntensity - 1 - (uint32_t) pixel[j]);
            }

            image->setPixel(x, y, pixel);
        }
    }

    return image;
};

//s = c*(r^gamma)
Image* TiffTransformer::intensityPowerlaw(Image* image, float gamma, uint32_t c) {
    uint32_t maxIntensity = pow(2, image->getBpc()) - 1;

    for (int x = 0; x < image->getHeight(); x++) {
        for (int y = 0; y < image->getWidth(); y++) {
            
            u_char* pixel = image->getPixel(x, y);
            for (int j = 0; j < image->getChannels(); j++) {
                uint32_t newIntensity = maxIntensity * pow((float) pixel[j] * 1.0 / maxIntensity, gamma);
                pixel[j] = (u_char) std::min(maxIntensity, newIntensity);
            }

            image->setPixel(x, y, pixel);
        }
    }

    return image;
};

Image* TiffTransformer::thresholdSingle(Image* image, uint32_t cutoff) {
    if (image->getChannels() > 1) {
        throw std::invalid_argument("Multichannel image thresholding is not supported.");
    }


    for (int x = 0; x < image->getHeight(); x++) {
        for (int y = 0; y < image->getWidth(); y++) {
            u_char* pixel = image->getPixel(x, y);

            if ((uint32_t)pixel[0] < cutoff) {
                pixel[0] = (u_char)0;
            } else {
                pixel[0] = (u_char)255;
            }

            image->setPixel(x, y, pixel);   
        }
    }

    return image;
};

Image* TiffTransformer::thresholdRange(Image* image, uint32_t floor, uint32_t ceil) {
    if (image->getChannels() > 1) {
        throw std::invalid_argument("Multichannel image thresholding is not supported.");
    }

    for (int x = 0; x < image->getHeight(); x++) {
        for (int y = 0; y < image->getWidth(); y++) {
            u_char* pixel = image->getPixel(x, y);

            if ((uint32_t)pixel[0] > floor && (uint32_t)pixel[0] < ceil) {
                pixel[0] = (u_char)255;
            } else {
                pixel[0] = (u_char)0;
            }

            image->setPixel(x, y, pixel);
        }
    }

    return image;
};

Image* TiffTransformer::intensityLUT(Image *image, u_char* lut) {
    for (int x = 0; x < image->getHeight(); x++) {
        for (int y = 0; y < image->getWidth(); y++) {
            u_char* pixel = image->getPixel(x, y);

            for (int j = 0; j < image->getChannels(); j++) {
                pixel[j] = (u_char) lut[pixel[j]];
            }

            image->setPixel(x, y, pixel);
        }
    }

    return image;
};

Image* TiffTransformer::histogramEqualization(Image* image) {
    vector<float> CDF = image->getCDF();
    uint32_t L = pow(2, image->getBpc());

    for (int x = 0; x < image->getHeight(); x++) {
        for (int y = 0; y < image->getWidth(); y++) {
            u_char* px = image->getPixel(x, y);
            cout<<CDF[px[0]]<<endl;
            px[0] = (u_char)round((L - 1) * CDF[px[0]]);
            // cout<<" "<<(uint32_t)px[0]<<endl;
            image->setPixel(x, y, px);
        }
    }

    return image;
};

FloatImage* TiffTransformer::spatialFilter(Image* image, vector<vector<float>> filter, SpatialFilterType type) {
    if (image->getChannels() > 1) {
        throw std::invalid_argument("Multichannel images are not supported for spatial filtering.");
    }

    Image* imgClone = image->clone();
    //convert to float image before applying spatial filters
    FloatImage* floatImg = this->toFloat(image);
    FloatImage* bufImg = this->toFloat(imgClone);

    for (int x = 0; x < image->getHeight(); x++) {
        for(int y = 0; y < image->getWidth(); y++) {
            float* px = this->applyFilter(floatImg, filter, x, y, type);
            bufImg->setPixel(x, y, px);
        }
    }

    // this->to8bit(bufImg, image, ScalingMode::minxmax);
    free(imgClone);
    free(floatImg);

    return bufImg;
};

FloatImage* TiffTransformer::sobelGradient(Image* image, vector<vector<float>> filterX, vector<vector<float>> filterY) {
    if (image->getChannels() > 1) {
        throw std::invalid_argument("Multichannel images are not supported for sobel gradient.");
    }
    Image* imgClone = image->clone();
    FloatImage* floatImg = this->toFloat(image);
    FloatImage* bufImg = this->toFloat(imgClone);
    float* G = (float*) malloc(sizeof(float));

    for (int x = 0; x < image->getHeight(); x++) {
        for(int y = 0; y < image->getWidth(); y++) {
            float* Gx = this->applyFilter(floatImg, filterX, x, y, SpatialFilterType::convolution);
            float* Gy = this->applyFilter(floatImg, filterY, x, y, SpatialFilterType::convolution);

            G[0] = sqrt(pow(Gx[0], 2) + pow(Gy[0], 2));

            bufImg->setPixel(x, y, G);
        }
    }

    free(imgClone);
    free(floatImg);

    return bufImg;
}

float* TiffTransformer::applyFilter(FloatImage* image, vector<vector<float>> filter, uint32_t x, uint32_t y, SpatialFilterType type) {
    int start = (filter.size() * (-1.0)) / (2);
    float result = 0;

    for (uint32_t w = 0; w < filter.size(); w++) {
        for (uint32_t u = 0; u < filter[w].size(); u++) {
            uint32_t correspondingX, correspondingY;
            if (type == SpatialFilterType::convolution) {
                correspondingX = x - (u + start);
                correspondingY = y - (w + start);
            } else {
                correspondingX = x + (u + start);
                correspondingY = y + (w + start);
            }
            if (
                correspondingX < 0 
                || correspondingY < 0
                || correspondingX >= image->getHeight()
                || correspondingY >= image->getWidth()
            ) {
                continue;
            }

            float* pixel = image->getPixel(correspondingX, correspondingY);

            result += pixel[0] * filter[w][u];
        }
    }

    float* newPixel = (float*) malloc(sizeof(float) * image->getChannels());
    newPixel[0] = result;

    return newPixel;
};

// assuming images are 8 bits
u_char* TiffTransformer::negationLUT() {
    u_char* lut = (u_char*) malloc(sizeof(u_char) * 255);
    uint32_t maxIntensity = 255;

    for (int i = 0; i < 256; i++) {
        lut[i] = (u_char) maxIntensity - i;
    }

    return lut;
};

u_char* TiffTransformer::powerLawLUT(float gamma, uint32_t c) {
    u_char* lut = (u_char*) malloc(sizeof(u_char) * 255);
    uint32_t maxIntensity = 255;

    for (int i = 0; i < 256; i++) {
        uint32_t pixel = maxIntensity * pow((float) i * 1.0 / maxIntensity, gamma);
        lut[i] = (u_char) std::min(maxIntensity, pixel);
    }

    return lut;
};

FloatImage* TiffTransformer::toFloat(Image *image) {
  uint32_t pixelAmount = image->getHeight() * image->getWidth() * image->getChannels();
  float* data = (float*) malloc(sizeof(float) * pixelAmount);
  float maxIntensity = 255.0;
  for (int i = 0; i < pixelAmount; i++) {
    data[i] = (float)image->getImageData()[i] * 1.0 / maxIntensity;
  }

  return new FloatImage(
    image->getWidth(),
    image->getHeight(),
    data,
    image->getChannels(),
    image->getBpc()
  );
};

void TiffTransformer::scaleConvertMinMax(FloatImage* floatImg, Image* image) {
  uint32_t pixelAmount = image->getHeight() * image->getWidth() * image->getChannels();
  float maxIntensity = 255.0;
  float minPx = min(floatImg->getImageData(), pixelAmount);
  float maxPx = max(floatImg->getImageData(), pixelAmount);

cout<<"sharpened min: "<<minPx<<", max: "<<maxPx<<endl;

  for (int x = 0; x < image->getHeight(); x++) {
    for(int y = 0; y < image->getWidth(); y++) {
        u_char* pixel = image->getPixel(x, y);
        float* floatpix = floatImg->getPixel(x, y);

        for (int k = 0; k < image->getChannels(); k++) {
            uint32_t placeholder = (uint32_t)((floatpix[k] - minPx) * 255.0 / (maxPx - minPx));
            pixel[k] = (u_char) placeholder;
        }

        image->setPixel(x, y, pixel);
    }
  }
};

void TiffTransformer::scaleConvertTruncate(FloatImage* floatImg, Image* image) {
  float minpx = min(floatImg->getImageData(), floatImg->pixelAmount());
  for (int x = 0; x < image->getHeight(); x++) {
    for(int y = 0; y < image->getWidth(); y++) {
        u_char* pixel = image->getPixel(x, y);
        float* floatpix = floatImg->getPixel(x, y);

        for (int k = 0; k < image->getChannels(); k++) {
            if (floatpix[k] < 0) {
                pixel[k] = (u_char) 0;
            } else if (floatpix[k] > 1) {
                pixel[k] = (u_char) 255;
            } else {
                uint32_t plc = (uint32_t) ((floatpix[k]) * 255.0);
                pixel[k] = (u_char) plc;
            }
        }
        image->setPixel(x, y, pixel);
    }
  }    
}

void TiffTransformer::to8bit(FloatImage* floatImg, Image* image, ScalingMode mode) {
  if (mode == ScalingMode::minmax) {
    this->scaleConvertMinMax(floatImg, image);
  } else if (mode == ScalingMode::truncate) {
    this->scaleConvertTruncate(floatImg, image);
  }
};

void TiffTransformer::scaleMinMax(uint32_t* data, uint32_t pixelAmount) {
//   float maxIntensity = 255.0;
  uint32_t minpx = min(data, pixelAmount), 
  maxpx = max(data, pixelAmount);

  for (int i = 0; i < pixelAmount; i++) {
    data[i] = (data[i] - minpx) * (255.0 / (maxpx - minpx) * 1.0);
  }
};

void TiffTransformer::scaleTruncate(uint32_t* data, uint32_t pixelAmount) {
  for (int i = 0; i < pixelAmount; i++) {
    if (data[i] < 0) {
        data[i] = 0;
    } else if (data[i] > 255) {
        data[i] = 255;
    }
  }
}

void TiffTransformer::addImages(FloatImage* image1, FloatImage* image2) {
    if (image1->getHeight() != image2->getHeight() || image1->getWidth() != image2->getWidth()) {
        throw std::invalid_argument("Image dimensions aren't the same");
    }
    uint32_t pxc = image1->pixelAmount();
    float minpx1 = min(image1->getImageData(), pxc);
    float minpx2 = min(image2->getImageData(), pxc);
    float maxpx1 = max(image1->getImageData(), pxc);
    float maxpx2 = max(image2->getImageData(), pxc);

    cout<<"min1: "<<minpx1<<", max1: "<<maxpx1<<endl;
    cout<<"min2: "<<minpx2<<", max2: "<<maxpx2<<endl;
    for (int x = 0; x < image1->getHeight(); x++) {
        for (int y = 0; y < image1->getWidth(); y++) {
            float* px1 = image1->getPixel(x, y);
            float* px2 = image2->getPixel(x, y);

            for (int k = 0; k < image1->getChannels(); k++) {
                px1[k] = px1[k] + px2[k];
            }

            image1->setPixel(x, y, px1);
        }
    }
};

void TiffTransformer::subtractImages(Image* image1, Image* image2) {
    if (image1->getHeight() != image2->getHeight() || image1->getWidth() != image2->getWidth()) {
        throw std::invalid_argument("Image dimensions aren't the same");
    }

        for (int x = 0; x < image1->getHeight(); x++) {
        for (int y = 0; y < image1->getWidth(); y++) {
            u_char* px1 = image1->getPixel(x, y);
            u_char* px2 = image2->getPixel(x, y);
            for (int k = 0; k < image1->getChannels(); k++) {
                px1[k] = px1[k] - px2[k];
            }

            image1->setPixel(x, y, px1);
        }
    }
}

void TiffTransformer::addImages(Image* image1, Image* image2, float k, ScalingMode mode) {
    if (image1->getHeight() != image2->getHeight() || image1->getWidth() != image2->getWidth()) {
        throw std::invalid_argument("Image dimensions aren't the same");
    }

    uint32_t* data = (uint32_t*) malloc(sizeof(uint32_t) * image1->getHeight() * image1->getWidth());
    int i = 0;

    for (int x = 0; x < image1->getHeight(); x++) {
        for (int y = 0; y < image1->getWidth(); y++) {
            u_char* px1 = image1->getPixel(x, y);
            u_char* px2 = image2->getPixel(x, y);

            for (int k = 0; k < image1->getChannels(); k++) {
                data[i] = (uint32_t) ((float)px1[k] + k * ((float) px2[k]));
                i++;
            }
        }
    }

    if (mode == ScalingMode::minmax) {
        this->scaleMinMax(data, i);
    } else if (mode == ScalingMode::truncate) {
        this->scaleTruncate(data, i);
    }
    uint32_t newmin = min(data, i);
    uint32_t newmax = max(data, i);

    cout<<"new sharpened min: "<<newmin<<", new sharp max: "<<newmax<<endl;
    i = 0;

    for (int x = 0; x < image1->getHeight(); x++) {
        for (int y = 0; y < image1->getWidth(); y++) {
            u_char* px = image1->getPixel(x, y);
            px[0] = data[i];
            image1->setPixel(x, y, px);
            i++;
        }
    }
};