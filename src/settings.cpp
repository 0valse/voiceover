#include <QNetworkProxy>

#include "settings.h"


bool Settings::is_key(QString tmp_key)
{
    QRegExp key_rx = QRegExp(KEY_RX);
    if (key_rx.indexIn(tmp_key) == 0) {
        return true;
    } else {
        return false;
    }
}

_appSettings Settings::loadAppSettings()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    QStringList lst, lst_tmp;
    QString lst_str = settings.value(SETTINGS_KEYS, "").toString();
    QString tmp;

    lst_tmp = lst_str.split(LIST_SEPARATOR, QString::SkipEmptyParts);
    lst = QStringList();

    for (int i = 0; i < lst_tmp.size(); ++i) {
        tmp = lst_tmp.at(i);
        if (this->is_key(tmp)) {
            lst.append(tmp);
        } else {
            qDebug() << "Not valid key: " + tmp;
            tmp.clear();
        }
    }

    _appSettings sets(
            settings.value(SETTINGS_PROXY_USE, false).toBool(),
            settings.value(SETTINGS_PROXY_TYPE, QNetworkProxy::HttpProxy).toInt(),
            settings.value(SETTINGS_PROXY_HOST, "").toString(),
            settings.value(SETTINGS_PROXY_PORT, 80).toInt(),
            settings.value(SETTINGS_PROXY_USERNAME, "").toString(),
            settings.value(SETTINGS_PROXY_PASSWORD, "").toString(),
            lst
    );
    return sets;
}

int Settings::saveAppSettings(_appSettings app_settings)
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    settings.setValue(SETTINGS_PROXY_USE, app_settings.used);
    settings.setValue(SETTINGS_PROXY_TYPE, app_settings.type);
    settings.setValue(SETTINGS_PROXY_HOST, app_settings.host);
    settings.setValue(SETTINGS_PROXY_PORT, app_settings.port);
    settings.setValue(SETTINGS_PROXY_USERNAME, app_settings.username);
    settings.setValue(SETTINGS_PROXY_PASSWORD, app_settings.password);
    settings.setValue(SETTINGS_KEYS,
                      app_settings.app_keys.join(LIST_SEPARATOR));

    settings.sync();
    return settings.status();
}

Settings::Settings() {}
Settings::~Settings() {}
