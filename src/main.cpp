#include <QApplication>
#include <QMainWindow>

#include "mainform.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    MainForm w;
    w.show();

    return app.exec();
}
