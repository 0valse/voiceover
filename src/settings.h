#include <QSettings>
#include <QRegExp>
#include <QNetworkProxy>

#ifndef SETTINGS_H
#define SETTINGS_H


/* Defining */
#define ORGANIZATION_NAME "Sasha Nenudny Inc"
#define ORGANIZATION_DOMAIN "ivoicer.online"
#define APPLICATION_NAME "ivoicer"
#define SETTINGS_PROXY_USE "proxy/used"
#define SETTINGS_PROXY_TYPE "proxy/type"
#define SETTINGS_PROXY_HOST "proxy/host"
#define SETTINGS_PROXY_PORT "proxy/port"
#define SETTINGS_PROXY_USERNAME "proxy/username"
#define SETTINGS_PROXY_PASSWORD "proxy/password"
#define SETTINGS_KEYS "ivoicer/keys"

#define LIST_SEPARATOR ':'
#define KEY_RX "[a-zA-Z0-9]{8}-[a-zA-Z0-9]{4}-[a-zA-Z0-9]{4}-[a-zA-Z0-9]{4}-[a-zA-Z0-9]{12}"


struct _appSettings {
    bool used;
    int type;
    QString host;
    int port;
    QString username;
    QString password;
    QStringList app_keys;
    _appSettings(bool us = false, int t = (int)QNetworkProxy::HttpProxy,
                  QString h = "", int p = 80, QString u = "", QString pswd = "",
                  QStringList keys = QStringList())
        : used(us), type(t), host(h), port(p), username(u), password(pswd), app_keys(keys)
        {}
};


class Settings
{
public:
    Settings();
    ~Settings();
    int saveAppSettings(_appSettings proxy_settings);
    _appSettings loadAppSettings();
    static bool is_key(QString tmp_key);
};

#endif // SETTINGS_H
