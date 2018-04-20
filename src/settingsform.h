#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QWidget>
#include <QList>
#include <QNetworkProxy>



struct MyProxyTypes {
    QList<int> type;
    QStringList name;
    MyProxyTypes(QList<int> t, QStringList n)
        : type(t), name(n) {}
};
static const struct MyProxyTypes myproxytypes (
    QList<int>()
        << QNetworkProxy::HttpProxy
        << QNetworkProxy::Socks5Proxy,
    QStringList()
        << "HTTPS"
        << "Socks5"
);


namespace Ui
{
class SettingsForm;
}


class SettingsForm : public QWidget
{
    Q_OBJECT
public:
    SettingsForm();
    ~SettingsForm();

private slots:
    void onSaveSettings();
    
private:
    Ui::SettingsForm* settings_ui;
    void struct_settings2widgets();
};

#endif // SETTINGSFORM_H
