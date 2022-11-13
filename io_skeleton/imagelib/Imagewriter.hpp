#ifndef SIL_TIFF_WRITER_HPP
#define SIL_TIFF_WRITER_HPP

#include <tiffio.h>
#include "imageloader.hpp"
#include <string>

class ImageWriter {
    public:
        void saveImageToFile(ImageLoader imageLoader, std::string fileName);
};

#endif