#include "webspider.h"
#include "webparser.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WebSpider w;
    w.show();

    return a.exec();
}
