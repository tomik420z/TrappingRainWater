#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QDebug>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QLocale::setDefault(QLocale::C);

    MainWindow w;
    w.show();

    return a.exec();
}
