#ifndef QT_IMAGE_VIEWER_MAINWINDOW_HPP
#define QT_IMAGE_VIEWER_MAINWINDOW_HPP

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QImage>
#include <QScrollArea>

#include <QSplitter>
#include <QChartView>

#include <image.hpp>

#include <vector>

class QtImageViewer : public QMainWindow{

	Q_OBJECT

public:
	QtImageViewer(QWidget *parent=nullptr);
	QtImageViewer(const QString filename, QWidget *parent=nullptr);
	
	virtual ~QtImageViewer();
			  
public slots:
	//void showFile(const QString filename);
	void showImageLeft(Image *img);
	void showImageRight(Image *img);
	void openFile();
	void quit();
private:
	enum ImageView{
		NONE = 0,
		LEFT = 2<<0,
		RIGHT = 2<<1,
	};

	void init();
	void createActions();
	void createMenus();
	
	QSplitter *_mainSplitter{nullptr};

	unsigned int _imageState{ImageView::NONE};

	void showImage(Image* img, ImageView view);
	
	QImage _lImage;
	QLabel *_lImageLabel{nullptr};
	QScrollArea *_lScrollArea{nullptr};
	QSplitter *_leftSplitter{nullptr};
	QtCharts::QChartView *_lChartView{nullptr};
	
	QImage _rImage;
	QLabel *_rImageLabel{nullptr};
	QScrollArea *_rScrollArea{nullptr};
	QSplitter *_rightSplitter{nullptr};
	QtCharts::QChartView *_rChartView{nullptr};
	
	QMenu *_fileMenu{nullptr};
	QAction *_fileOpenAction{nullptr};
	QAction *_quitAction{nullptr};
	
};

#endif
