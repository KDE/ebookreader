#include <main.h>
#include <okulardocument.h>
#include <QApplication>

#define PDF_FILE "/home/bogdan/Biblio/eBooks/umbrello.pdf"


void TestTabletReader::openPDF()
{
	int argc = 0;
	QApplication app(argc, NULL);
	OkularDocument doc;
	QCOMPARE(EXIT_SUCCESS, doc.load(PDF_FILE));
	QVERIFY(NULL != doc.getPixmap(0, -1,  -1));
}

QTEST_MAIN(TestTabletReader);
