#include <QSettings>

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





struct ProxySettings {
    bool used;
    int type;
    QString host;
    int port;
    QString username;
    QString password;
    ProxySettings(bool us = false, int t = QNetworkProxy::HttpProxy,
                  QString h = "", int p = 80, QString u = "", QString pswd = "")
        : used(us), type(t), host(h), port(p), username(u), password(pswd)
        {}
};


class Settings
{
public:
    Settings();
    ~Settings();
    int saveProxySettings(ProxySettings proxy_settings);
    ProxySettings loadProxySettings();
};

#endif // SETTINGS_H
