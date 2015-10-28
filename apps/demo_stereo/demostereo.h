#ifndef DEMOSTEREO_H
#define DEMOSTEREO_H

#include <QMainWindow>
#include <spider.h>
#include <platformLinux.h>

namespace Ui {
	class DemoStereo;
}

class DemoStereo : public QMainWindow
{
	Q_OBJECT

public:
	explicit DemoStereo(QWidget *parent = 0);
	void getParams(Param* minDisp, Param* maxDisp, Param* scale, Param* nIter, Param* nSlice);
	~DemoStereo();

private slots:
	void startSpider();
	void openGantt();
	void openSrdag();
	void help();

private:
	void initSpider();
	void cleanSpider();
	QString convertToUnits(int val);
	QString convertTime(Time t);
	bool checkParams();

	Ui::DemoStereo *ui;

	QPixmap *pixmapL, *pixmapR, *pixmapD;

	StaticStack *pisdfStack, *archiStack;
	PlatformLinux *platform;
	Archi* archi;
	PiSDFGraph *topPisdf;
};

#endif // DEMOSTEREO_H
