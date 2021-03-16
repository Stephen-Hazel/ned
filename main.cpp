#include "ned.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    NEd w;
    w.show();
    return a.exec();
}
