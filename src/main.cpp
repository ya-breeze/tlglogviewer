#include <iostream>

#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    try {
        QApplication a(argc, argv);
        MainWindow w;
        w.show();

        if( argc>=2 )
            w.load(argv[1]);

        return a.exec();
    } catch (const std::exception& _ex) {
        std::cerr << "ERROR: " << _ex.what() << std::endl;
    }

    return -1;
}
