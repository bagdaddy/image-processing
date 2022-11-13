#ifndef SIL_TIFF_WRITER_HPP
#define SIL_TIFF_WRITER_HPP

#include <tiffio.h>
#include "tiffloader.hpp"
#include <string>

class TiffWriter {
    public:
        void saveImage(Image* image, std::string fileName);
        TIFF* prepareTiff(Image* image, std::string fileName);
};

#endif