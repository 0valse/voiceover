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

    QString _in_file = "";
    QStringList app_args = app.arguments();
    app_args.takeFirst(); // del appname

    if (app_args.size() >= 1) _in_file = app_args.first();

    MainForm w(0, _in_file);
    w.show();

    return app.exec();
}
