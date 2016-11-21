#include "mainwindow.h"
#include "tablewindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    TableWindow second;
    second.show();

    return a.exec();
}
