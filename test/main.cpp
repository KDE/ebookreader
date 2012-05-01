#include <main.h>
#include <okulardocument.h>
#include <chmdocument.h>
#include <QApplication>

#define PDF_FILE "/home/bogdan/Biblio/eBooks/umbrello.pdf"
#define DJVU_FILE "/home/bogdan/Biblio/eBooks/Linear Programming - Foundation and Extensions 2nd ed. - R. vanderbei.djvu"
#define EPUB_FILE "/home/bogdan/Biblio/eBooks/JulesVerne/verne-mysterious-island.epub"
#define CHM_FILE "/home/bogdan/Biblio/eBooks/oram07beautiful.chm"

void TestTabletReader::openPDF()
{
	openFile(PDF_FILE);
}

void TestTabletReader::openDJVU()
{
	openFile(DJVU_FILE);
}

void TestTabletReader::openEPUB()
{
	openFile(EPUB_FILE);
}

void TestTabletReader::openCHM()
{
	openFile(CHM_FILE, true);
}

void TestTabletReader::openFile(const char *fileName, bool useCHM)
{
	int argc = 1;
	char *argv[] = {"testTabletReader"};
	QApplication app(argc, argv);//needed by OkularDocument class
	Document *doc = NULL;
	if (true == useCHM)
	{
		doc = new CHMDocument();
	} else {
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
		pixmap = doc->getPixmap(n, 800, -1);
		QVERIFY(NULL != pixmap);
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
		pixmaps[n%3] = doc->getPixmap(n, -1, -1);
		QVERIFY(NULL != pixmaps[0]);
	}
	delete doc;
}

QTEST_MAIN(TestTabletReader);
