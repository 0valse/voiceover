#include <QApplication>
#include <QMainWindow>

#include "mainform.h"
#include "settings.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setOrganizationDomain(ORGANIZATION_DOMAIN);
    QCoreApplication::setApplicationName(APPLICATION_NAME);
    

    MainForm w;
    w.show();

    return app.exec();
}
