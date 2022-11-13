#include <tiffio.h>
#include <iostream>
#include "tiffwriter.hpp"
#include "tiffloader.hpp"
#include <string.h>

using namespace std;

void TiffWriter::saveImage(Image* image, std::string fileName) {
    TIFF *tiff = prepareTiff(image, fileName);

    uint32_t stripSize = image->getWidth() * image->getChannels();

    u_char* scan_line = (u_char *) _TIFFmalloc(stripSize);
    u_char* data = image->getImageData();

    int j;

    for (int i = 0; i < image->getHeight(); i++) //loading the data into a buffer
    {
        for (j = 0; j < stripSize; j++) {
            scan_line[j] = data[i * image->getWidth() * image->getChannels() + j];
        }

        TIFFWriteScanline(tiff, scan_line, i, 0);
    }

    TIFFClose(tiff);
    free(scan_line);
};

TIFF* TiffWriter::prepareTiff(Image* image, std::string fileName) {
    char fileNameC[fileName.length() + 1];

    strcpy(fileNameC, fileName.c_str());

    TIFF *tiff = TIFFOpen(fileNameC, "w");

    if (image->getChannels() > 1) {
        TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    }


    TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, image->getHeight());
    TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, image->getWidth());
    TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, image->getChannels());
    TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, image->getBpc());
    TIFFSetField(tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_INT);
    TIFFSetField(tiff, TIFFTAG_COMPRESSION, COMPRESSION_JPEG);

    return tiff;
}