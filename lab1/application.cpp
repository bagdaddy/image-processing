#include <iomanip>
#include <iostream>
#include <vector>
#include "image.hpp"
#include "tiffloader.hpp"
#include "tiffTransformer.hpp"
#include "tiffwriter.hpp"

using namespace std;

class Application {
    public:
        Application() {
            TiffTransformer* _tiffTransformer;
            TiffWriter* _tiffWriter;

            this->tiffTransformer = _tiffTransformer;
            this->tiffWriter = _tiffWriter;
        };

        void run(TiffLoader* tiffLoader) {
            Image* image = tiffLoader->getImage();
            tiffLoader->printMetaData();

            Image* transformedImage = image->clone();
            uint32_t command = 0;

            do {
                command = getCommand();
                switch(command) {
                    case 0:
                        this->saveImage(transformedImage);
                        break;
                    case 1:
                        this->inverse(transformedImage);
                        break;
                    case 2:
                        this->powerlaw(transformedImage);
                        break;
                    case 3:
                        this->thresholdSingle(transformedImage);
                        break;
                    case 4:
                        this->thresholdSingle(transformedImage);
                        break;
                    case 5:
                        this->inverseLUT(transformedImage);
                        break;
                    case 6:
                        this->powerlawLUT(transformedImage);
                        break;
                    case 7:
                        this->averagingFilter(transformedImage);
                        break;
                    case 8:
                        this->laplacian4n(transformedImage);
                        break;
                    case 9:
                        this->laplacian8n(transformedImage);
                        break;
                    case 10:
                        this->unsharpMaskSharpening(transformedImage);
                        break;
                    case 11:
                        this->sobelFiltering(transformedImage);
                        break;
                    case 12:
                        this->sobelInvFiltering(transformedImage);
                        break;
                    case 13:
                        this->histogramEqualization(transformedImage);
                        break;
                    default:
                        continue;
                }
            } while(command != 0);
        };

        void inverse(Image* image) {
            this->tiffTransformer->intensityNegation(image);
        }

        void inverseLUT(Image* image) {
            u_char* lut = this->tiffTransformer->negationLUT();
            this->tiffTransformer->intensityLUT(image, lut);
        }

        void powerlaw(Image* image) {
            uint32_t gamma;

            cout<<"Enter gamma: "<<endl;
            cin>>gamma;

            this->tiffTransformer->intensityPowerlaw(image, gamma, 1);
        }

        void powerlawLUT(Image *image) {
            uint32_t gamma;
            cout<<"Enter gamma: "<<endl;
            cin>>gamma;

            u_char* lut = this->tiffTransformer->powerLawLUT(gamma, 1);
            this->tiffTransformer->intensityLUT(image, lut);
        }

        void thresholdSingle(Image* image) {
            uint32_t cutoff;
            cout<<"Enter cutoff for thresholding: "<<endl;
            cin>>cutoff;

            this->tiffTransformer->thresholdSingle(image, cutoff);
        };

        void histogramEqualization(Image* image) {
            this->tiffTransformer->histogramEqualization(image);
        };

        void thresholdRange(Image* image) {
            uint32_t lb, ub;
            cout<<"Enter lower bound for thresholding: "<<endl;
            cin>>lb;
            cout<<"Enter upper bound for thresholding: "<<endl;
            cin>>ub;

            this->tiffTransformer->thresholdRange(image, lb, ub);
        };

        void averagingFilter(Image* image) {
            uint32_t size, type;
            cout<<endl;
            cout<<"Input size of the averaging filter: "<<endl;
            cin>>size;
            vector<float> row(size, 1.0/(size*size*1.0));
            vector<vector<float>> filter (size, row);

            cout<<"Choose method (correlation: 0, convolution: 1): "<<endl;
            cin>>type;

            this->tiffTransformer->spatialFilter(image, filter, static_cast<SpatialFilterType>(type));
        };

        void laplacian8n(Image* image) {
            vector<vector<float>> laplacianFilter
            {
                {-1.0, -1.0, -1.0},
                {-1.0, 8.0, -1.0},
                {-1.0, -1.0, -1.0}
            };

            this->applySpatialFilter(image, laplacianFilter);
        };

        void laplacian4n(Image* image) {
            vector<vector<float>> laplacianFilter
            {
                {0.0, -1.0, 0.0},
                {-1.0, 4.0, -1.0},
                {0.0, -1.0, 0.0}
            };

            this->applySpatialFilter(image, laplacianFilter);
        };

        void applySpatialFilter(Image* image, vector<vector<float>> filter) {
            uint32_t type;
            cout<<"Choose method (correlation: 0, convolution: 1): "<<endl;
            cin>>type;

            Image* mask8bit = image->clone();

            FloatImage* mask = this->tiffTransformer->spatialFilter(image, filter, static_cast<SpatialFilterType>(type));

            this->tiffTransformer->to8bit(mask, mask8bit, ScalingMode::minmax);

            this->tiffTransformer->addImages(image, mask8bit, 1, ScalingMode::minmax);
        };

        void unsharpMaskSharpening(Image* image) {
            vector<float> row(3, 1.0/(9.0));
            vector<vector<float>> filter (3, row);

            Image* subMask = image->clone();
            Image* mask = image->clone();

            FloatImage* flMask = this->tiffTransformer->spatialFilter(image, filter, SpatialFilterType::correlation);

            this->tiffTransformer->to8bit(flMask, subMask, ScalingMode::minmax);
            this->tiffTransformer->subtractImages(mask, subMask);
            this->tiffTransformer->addImages(image, mask, 2, ScalingMode::minmax);
        };

        void sobelFiltering(Image* image) {
            vector<vector<float>> filter
            {
                {-1.0, 0.0, 1.0},
                {-2.0, 0.0, 2.0},
                {-1.0, 0.0, 1.0}
            };

            vector<vector<float>> filterY
            {
                {-1.0, -2.0, -1.0},
                {0.0, 0.0, 0.0},
                {1.0, 2.0, 1.0}
            };

            FloatImage* flGradient = this->tiffTransformer->sobelGradient(image, filter, filterY);
            this->tiffTransformer->to8bit(flGradient, image, ScalingMode::minmax);    //truncate produces a clearer and more contrasted edge, than minmax
        };

        void sobelInvFiltering(Image* image) {
            vector<vector<float>> filter
            {
                {-1.0, -2.0, -1.0},
                {0.0, 0.0, 0.0},
                {1.0, 2.0, 1.0}
            };

            FloatImage* flMask = this->tiffTransformer->spatialFilter(image, filter, SpatialFilterType::convolution);
            Image* clone = image->clone();

            this->tiffTransformer->to8bit(flMask, image, ScalingMode::truncate);
            this->tiffTransformer->subtractImages(clone, image);

            image = clone;
        };

        void saveImage(Image* image) {
            string outputName;

            cout<<"Enter output file name: "<<endl;
            cin>>outputName;

            this->tiffWriter->saveImage(image, "output/" + outputName + ".tif");
        };

        int getCommand() {
            uint32_t command;
            cout<<endl<<endl;
            cout<<"Choose a transformation: "<<endl;
            cout<<"0. Save"<<endl;
            cout<<"1. Inverse"<<endl;
            cout<<"2. Intensity Power Law"<<endl;
            cout<<"3. Thresholding (single)"<<endl;
            cout<<"4. Thresholding (range)"<<endl;
            cout<<"5. Inverse using LUT"<<endl;
            cout<<"6. Power Law using LUT"<<endl;
            cout<<"7. Averaging (blur)"<<endl;
            cout<<"8. Sharpen 4-n laplacian"<<endl;
            cout<<"9. Sharpen 8-n laplacian"<<endl;
            cout<<"10. Unsharp sharpening"<<endl;
            cout<<"11. Sobel filter"<<endl;
            cout<<"12. Sobel filter (inverse)"<<endl;
            cout<<"13. Histogram equalization"<<endl;

            cin>>command;

            return command;
        };

    private:
        TiffWriter* tiffWriter;
        TiffTransformer* tiffTransformer;
};