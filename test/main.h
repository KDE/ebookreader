#include <QtTest/QtTest>

class TestTabletReader : public QObject
{
	Q_OBJECT
private:
	void openFile(const char* fileName, bool useCHM = false);
private slots:
	void openPDF();
	void openDJVU();
	void openEPUB();
	void openCHM();
	void openTIF();
	void openDVI();
	void openPS();
	void openJPG();
	void openODT();
	void singleApp();
};

