#include <QNetworkProxy>

#include "settings.h"


ProxySettings Settings::loadProxySettings()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    ProxySettings sets(
            settings.value(SETTINGS_PROXY_USE, false).toBool(),
            settings.value(SETTINGS_PROXY_TYPE, QNetworkProxy::HttpProxy).toInt(),
            settings.value(SETTINGS_PROXY_HOST, "").toString(),
            settings.value(SETTINGS_PROXY_PORT, 80).toInt(),
            settings.value(SETTINGS_PROXY_USERNAME, "").toString(),
            settings.value(SETTINGS_PROXY_PASSWORD, "").toString()
    );
    return sets;
}

int Settings::saveProxySettings(ProxySettings proxy_settings)
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    settings.setValue(SETTINGS_PROXY_USE, proxy_settings.used);
    settings.setValue(SETTINGS_PROXY_TYPE, proxy_settings.type);
    settings.setValue(SETTINGS_PROXY_HOST, proxy_settings.host);
    settings.setValue(SETTINGS_PROXY_PORT, proxy_settings.port);
    settings.setValue(SETTINGS_PROXY_USERNAME, proxy_settings.username);
    settings.setValue(SETTINGS_PROXY_PASSWORD, proxy_settings.password);
    
    settings.sync();
    return settings.status();
}

Settings::Settings() {}
Settings::~Settings() {}
