#include <main.h>
#include <okulardocument.h>
#include <chmdocument.h>
#include <QApplication>
#include <core/settings_core.h>
#include "SingleApp.h"

#define PDF_FILE "/home/bogdan/Biblio/eBooks/umbrello.pdf"
#define DJVU_FILE "/home/bogdan/Biblio/eBooks/Linear Programming - Foundation and Extensions 2nd ed. - R. vanderbei.djvu"
#define EPUB_FILE "/home/bogdan/Biblio/eBooks/JulesVerne/verne-mysterious-island.epub"
#define CHM_FILE "/home/bogdan/Biblio/eBooks/oram07beautiful.chm"
#define TIF_FILE "/home/bogdan/Biblio/OkularSamples/CCITT_8.TIF"
#define DVI_FILE "/home/bogdan/Biblio/OkularSamples/CV-euro_en.dvi"
#define PS_FILE "/home/bogdan/Biblio/OkularSamples/CV-euro_en.ps"
#define JPG_FILE "/home/bogdan/Biblio/OkularSamples/example1.jpg"
#define ODT_FILE "/home/bogdan/Biblio/OkularSamples/example1.odt"

void TestTabletReader::openPDF()
{
#ifdef PDF_FILE
	openFile(PDF_FILE);
#else
#pragma message("No PDF file")
#endif
}

void TestTabletReader::openDJVU()
{
#ifdef DJVU_FILE
	openFile(DJVU_FILE);
#else
#pragma message("No DJVU file")
#endif
}

void TestTabletReader::openEPUB()
{
#ifdef EPUB_FILE
	openFile(EPUB_FILE);
#else
#pragma message("No EPUB file")
#endif
}

void TestTabletReader::openCHM()
{
#ifdef CHM_FILE
	openFile(CHM_FILE, true);//TODO: solve this exception
#else
#pragma message("No CHM file")
#endif
}

void TestTabletReader::openTIF()
{
#ifdef TIF_FILE
	openFile(TIF_FILE, false);
#else
#pragma message("No TIF file")
#endif
}

void TestTabletReader::openDVI()
{
#ifdef DVI_FILE
	openFile(DVI_FILE, false);
#else
#pragma message("No DVI file")
#endif
}

void TestTabletReader::openPS()
{
#ifdef PS_FILE
	openFile(PS_FILE, false);
#else
#pragma message("No PS file")
#endif
}

void TestTabletReader::openJPG()
{
#ifdef JPG_FILE
	openFile(JPG_FILE, false);
#else
#pragma message("No JPG file")
#endif
}

void TestTabletReader::openODT()
{
#ifdef ODT_FILE
	openFile(ODT_FILE, false);
#else
#pragma message("No ODT file")
#endif
}

void TestTabletReader::openFile(const char *fileName, bool useCHM)
{
	int argc = 1;
	char *argv[] = {(char*)"testTabletReader"};
	QApplication app(argc, argv);//needed by OkularDocument class
	Document *doc = NULL;
	if (true == useCHM)
	{
		doc = new CHMDocument();
	} else {
		Okular::SettingsCore::instance("");
		doc = new OkularDocument();
	}	
	QCOMPARE(EXIT_SUCCESS, doc->load(fileName));
	int numPages = doc->numPages();
	QVERIFY(0 != numPages);
	if (true == useCHM)
	{
		delete doc;
		return;//TODO: cannot get pages with CHM docs
	}
	const QPixmap *pixmap = NULL;

	//get at most 10 pages one by one
	numPages = (10 < numPages)?10:numPages;
	int n = 0;
	for (n = 0; n < numPages; ++n)
	{
		pixmap = doc->getPixmap(n, 1);
		QVERIFY(NULL != pixmap);
		QVERIFY2(false == pixmap->isNull(), "one page");
		doc->deletePixmap(pixmap);
	}

	//get three pages each time by deleting one when a fourth is generated
	const QPixmap *pixmaps[3] = {NULL};
	for (n = 0; n < numPages; ++n)
	{
		if (3 <= n)
		{
			doc->deletePixmap(pixmaps[n%3]);
		}
		pixmaps[n%3] = doc->getPixmap(n, 1);
		QVERIFY(NULL != pixmaps[n%3]);
		QVERIFY2(false == pixmaps[n%3]->isNull(), "three pages");
	}
	delete doc;
}

void TestTabletReader::singleApp()
{
	SingleApp single;
	QVERIFY(false == single.isRunning());
	QVERIFY(true == single.isRunning());
}

QTEST_MAIN(TestTabletReader);
