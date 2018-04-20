#include <QNetworkProxy>
#include <QSettings>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>

#include "settingsform.h"
#include "settings.h"

#include "ui_settingsform.h"

SettingsForm::SettingsForm()
    : settings_ui(new Ui::SettingsForm)
{
    settings_ui->setupUi(this);    
    for (int i = 0; i < myproxytypes.type.size(); ++i) {
        settings_ui->comboBoxProxyType->addItem(myproxytypes.name[i]);
    }
    this->struct_settings2widgets();

    connect(settings_ui->pushButtonSave, &QPushButton::clicked,
            this, &SettingsForm::onSaveSettings);
}

SettingsForm::~SettingsForm()
{
    delete settings_ui;
}


void SettingsForm::onSaveSettings()
{   
    Settings settings;
    ProxySettings proxy_settings = settings.loadProxySettings();

    proxy_settings.used = settings_ui->checkBoxProxyUsed->isChecked();
    proxy_settings.type = myproxytypes.type[settings_ui->comboBoxProxyType->currentIndex()];
    proxy_settings.host = settings_ui->lineEditProxyIP->text();
    proxy_settings.port = settings_ui->spinBoxProxyPort->value();
    proxy_settings.username = settings_ui->lineEditUser->text();
    proxy_settings.password = settings_ui->lineEditPassword->text();
    
    int ret = settings.saveProxySettings(proxy_settings);
    
    qDebug() << ret;
    
    switch (ret) {
        case QSettings::NoError: {
            this->close();
            break;
        }
        case QSettings::AccessError: {
            QMessageBox::critical(
                this,
                QString::fromUtf8("Ошибка сохранения конфигурации"),
                QString::fromUtf8("Не удалось сохранить конфигурацию. Нет доступа!")
            );
            break;
        }
        case QSettings::FormatError: {
            QMessageBox::critical(
                this,
                QString::fromUtf8("Ошибка сохранения конфигурации"),
                QString::fromUtf8("Не удалось сохранить конфигурацию. Ошибка формата данных!")
            );
            break;
        }
    }
    
}


void SettingsForm::struct_settings2widgets()
{
    Settings settings;
    ProxySettings proxy_settings = settings.loadProxySettings();
    
    settings_ui->checkBoxProxyUsed->setChecked(proxy_settings.used);
    settings_ui->lineEditProxyIP->setText(proxy_settings.host);
    settings_ui->comboBoxProxyType->setCurrentIndex(
        myproxytypes.type.indexOf(proxy_settings.type)
    );
    settings_ui->spinBoxProxyPort->setValue(proxy_settings.port);
    settings_ui->lineEditUser->setText(proxy_settings.username);
    settings_ui->lineEditPassword->setText(proxy_settings.password);
}

