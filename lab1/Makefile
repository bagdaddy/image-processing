LIBS =  -ltiff
TARGET = main
LIBDIR = /home/mantas/studies/image_processing/io_skeleton/imagelib

.PHONY: all clean

$(TARGET):
	g++ helpers.cpp tiffTransformer.cpp application.cpp main.cpp -I $(LIBDIR) $(LIBDIR)/libimagelib.so -o $(TARGET) $(LIBS)

clean:
	rm $(TARGET)
