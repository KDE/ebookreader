#include <main.h>
#include <okulardocument.h>

#define PDF_FILE "/home/bogdan/Biblio/eBooks/umbrello.pdf"


void TestTabletReader::openPDF()
{
	OkularDocument doc;
	QCOMPARE(EXIT_SUCCESS, doc.load(PDF_FILE));
	QVERIFY(NULL != doc.getPixmap(0, -1,  -1));
}

QTEST_MAIN(TestTabletReader);
