#include "demostereo.h"
#include "ui_demostereo.h"
#include <QtGui>
#include <QProcess>
#include <QMessageBox>

#include <spider.h>
#include <stereo.h>
#include <unistd.h>

#define SRDAG_SIZE 		8	*1024*1024
#define TRANSFO_SIZE 	8	*1024*1024
#define PISDF_SIZE 		4	*1024*1024
#define ARCHI_SIZE 		512	*1024

char transfoStack[TRANSFO_SIZE];
char srdagStack[SRDAG_SIZE];
char pisdfStackMem[PISDF_SIZE];
char archiStackMem[ARCHI_SIZE];

void DemoStereo::initSpider(){
	SpiderConfig cfg;

	pisdfStack = new StaticStack("PisdfStack", pisdfStackMem, PISDF_SIZE);
	archiStack = new StaticStack("ArchiStack", archiStackMem, ARCHI_SIZE);

	/* Retreive Pe count in current PC */
	int nPeMax = sysconf(_SC_NPROCESSORS_ONLN);
	ui->nPe->setMaximum(nPeMax);

	try{
#define SH_MEM 0x03000000
		platform = new PlatformLinux(nPeMax, SH_MEM, archiStack, stereo_fcts, N_FCT_STEREO);
	}catch(char* msg){
		printf("Spider error: %s\n", msg);
		return;
	}

	archi = platform->getArchi();

	cfg.memAllocType = MEMALLOC_SPECIAL_ACTOR;
	cfg.memAllocStart = (void*)0;
	cfg.memAllocSize = SH_MEM;

	cfg.schedulerType = SCHEDULER_LIST;

	cfg.srdagStack.type = STACK_STATIC;
	cfg.srdagStack.name = "SrdagStack";
	cfg.srdagStack.size = SRDAG_SIZE;
	cfg.srdagStack.start = srdagStack;

	cfg.transfoStack.type = STACK_STATIC;
	cfg.transfoStack.name = "TransfoStack";
	cfg.transfoStack.size = TRANSFO_SIZE;
	cfg.transfoStack.start = transfoStack;

	cfg.useGraphOptim = true;
	cfg.useActorPrecedence = true;

	spider_init(cfg);

	pisdfStack->freeAll();
	topPisdf = init_stereo(archi, pisdfStack);
}

QString DemoStereo::convertToUnits(int val) {
	QString unit;
	double value = val;

	if(value >= 1024*1024*1024) {
		value = value/(1024*1024*1024);
		unit = "G";
	}else if(value >= 1024*1024) {
		value = value/(1024*1024);
		unit = "M";
	}else if(value >= 1024){
		value = value/1024;
		unit = "K";
	}

	return (QString::number(value,'f',2)+" "+unit);
}

QString DemoStereo::convertTime(Time t) {
	QString unit;
	double value = t;

	if(value >= 1000000000) {
		value = value/(1000000000);
		unit = "s";
	}else if(value >= 1000000) {
		value = value/(1000000);
		unit = "ms";
	}else if(value >= 1000) {
		value = value/1000;
		unit = "µs";
	}else{
		value = value;
		unit = "ns";
	}

	return (QString::number(value,'f',3)+" "+unit);
}

void DemoStereo::openGantt(){
	QProcess::startDetached("GanttDisplay.jar -f stereo.pgantt");
}

void DemoStereo::openSrdag(){
	QProcess::startDetached("xdot stereo.gv");
}

void DemoStereo::startSpider(){
	ExecutionStat stat;

	if(!checkParams()) return;

	/* Set Active Pes */
	for(int i=0; i<ui->nPe->value(); i++)
		archi->activatePE(i);
	for(int i=ui->nPe->value(); i<archi->getNPE(); i++)
		archi->desactivatePE(i);

	Platform::get()->rstTime();

	try{
		spider_launch(archi, topPisdf);
	}catch(const char* msg){
		QMessageBox::critical(this, "Spider Error", msg);
		printf("Spider error: %s\n", msg);
		return;
	}

	spider_printGantt(archi, spider_getLastSRDAG(), "stereo.pgantt", "stereo.tex", &stat);
	spider_getLastSRDAG()->print("stereo.gv");

	/* Set Spider to Idle Mode */
	spider_idle(archi);

	/* Populate result field */
	ui->execTime->setText(convertTime(stat.globalEndTime));
	ui->schedTime->setText(convertTime(stat.schedTime));
	ui->mapTime->setText(convertTime(stat.mappingTime));
	ui->graphTime->setText(convertTime(stat.graphTime));
	ui->optimTime->setText(convertTime(stat.optimTime));

	ui->nSrdagActors->setText(QString::number(stat.nSRDAGActor));
	ui->nSrdagEdges->setText(QString::number(stat.nSRDAGEdge));
	ui->shMem->setText(convertToUnits(stat.memoryUsed)+"B");

	pixmapD->load("result.ppm","PPM");
	ui->imageDepth->setPixmap(*pixmapD);
}

void DemoStereo::cleanSpider(){
	free_stereo(topPisdf, pisdfStack);
	spider_free();
	delete platform;
	delete pisdfStack;
	delete archiStack;
}

DemoStereo::DemoStereo(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::DemoStereo)
{
	ui->setupUi(this);

	pixmapL = new QPixmap("im2.ppm","PPM");
	if(pixmapL->isNull()){
		printf("Failed to open im2.ppm");
		ui->imageL->setText("Failed to open im2.ppm");
	}else
		ui->imageL->setPixmap(*pixmapL);

	pixmapR = new QPixmap("im5.ppm","PPM");
	if(pixmapR->isNull()){
		printf("Failed to open im7.ppm");
		ui->imageR->setText("Failed to open im5.ppm");
	}else
		ui->imageR->setPixmap(*pixmapR);

	pixmapD = new QPixmap("result.ppm","PPM");
	if(pixmapD->isNull()){
		printf("Failed to open result.ppm");
		ui->imageDepth->setText("Failed to open result.ppm");
	}else
		ui->imageDepth->setPixmap(*pixmapD);

	/* Connect Buttons */
	connect(ui->buttonRun, SIGNAL (released()), this, SLOT (startSpider()));
	connect(ui->buttonGantt, SIGNAL (released()), this, SLOT (openGantt()));
	connect(ui->buttonSrdag, SIGNAL (released()), this, SLOT (openSrdag()));
	connect(ui->buttonHelp, SIGNAL (released()), this, SLOT (help()));

	/* Init Spider*/
	initSpider();
}

void DemoStereo::help(){
	QMessageBox msgBox;
	QSpacerItem* horizontalSpacer = new QSpacerItem(700, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
	msgBox.setText( "Some informations on parameters:\n"
					"    - Active PE: Number of PE to use for the execution\n"
					"    - minDisp: Minimal disparity value, usually set to zero for background\n"
					"    - maxDisp: Maximal disparity value, impact the detection of nearest pixels (20 in this image)\n"
					"    - scale: Coefficient that multiply the disparity value for the display (maxDisp*scale < 256)\n"
					"    - nIter: Iterations number on pixel, increasing it ameliorate the result\n"
					"    - nSlice: Number of slice used for the median filters\n" );
	QGridLayout* layout = (QGridLayout*)msgBox.layout();
	layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());
	msgBox.exec();
}

void DemoStereo::getParams(Param* minDisp, Param* maxDisp, Param* scale, Param* nIter, Param* nSlice){
	*minDisp = ui->minDisp->value();
	*maxDisp = ui->maxDisp->value();
	*nIter = ui->nIter->value();
	*nSlice = ui->nSlice->value();
	*scale = ui->scale->value();
}

bool DemoStereo::checkParams(){
	Param minDisp, maxDisp, nIter, nSlice, scale;
	getParams(&minDisp, &maxDisp, &scale, &nIter, &nSlice);

	if(minDisp >= maxDisp){
		QMessageBox::critical(this, "Bad Parameters", "minDisp must be lower than maxDisp");
		return false;
	}
	if(380 % nSlice != 0){
		QMessageBox::critical(this, "Bad Parameters", "nSlice must divide the height");
		return false;
	}
	if(scale*maxDisp >= 256){
		QMessageBox::critical(this, "Bad Parameters", "Scale too big (scale*maxDisp < 256)");
		return false;
	}
	return true;
}

DemoStereo::~DemoStereo()
{
	cleanSpider();
	delete pixmapL;
	delete pixmapR;
	delete pixmapD;
	delete ui;
}
