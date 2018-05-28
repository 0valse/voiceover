#include "settings.h"


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
    int key_count();

private slots:
    void onSaveSettings();
    void onKeyAdd();
    void onDeleteItem();
    void onItemChanged(int row);
    void onLineEditKeytextChanged(QString text);

private:
    Ui::SettingsForm* settings_ui;
    void struct_settings2widgets();
    Settings settings;
    _appSettings app_settings;
    bool _is_key_in_list(QString text);
};

#endif // SETTINGSFORM_H
