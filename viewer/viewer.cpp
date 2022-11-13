#include "qtimageviewer.hpp"

#include <tiffloader.hpp>

#include <QApplication>
#include <iostream>

int main(int argc, char **argv) {

	// Start Qt framework
	QApplication app(argc, argv);

	QtImageViewer *imv = new QtImageViewer();
	// Optionally give image on command line
	if (argc >= 2) {
		QString filename(argv[1]);
		std::cout << "Load directly: " << filename.toStdString() << std::endl;

		// Load tiff using your library!
		TiffLoader *tiffLoader = new TiffLoader(filename.toStdString());
		Image *myImage = tiffLoader->loadImage();
		tiffLoader->printMetaData();

		std::cout<<myImage->getBpc()<<std::endl;

		imv->showImageLeft(myImage);
		delete myImage;
	}

	if (argc >= 3) {
		QString filename(argv[2]);
		std::cout << "Load directly on the right: " << filename.toStdString() << std::endl;

		// Load tiff using your library!
		TiffLoader *tiffLoader = new TiffLoader(filename.toStdString());
		Image *myImage = tiffLoader->loadImage();
		tiffLoader->printMetaData();

		imv->showImageRight(myImage);
		delete myImage;
	}

	// Draw our GUI
	imv->show();
	imv->resize(1000, 600);

	// Run Qt application until someone presses quit
	int ret = app.exec();

	delete (imv);
	return ret;
}
