#include <QNetworkProxy>
#include <QSettings>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>
#include <QRegExpValidator>
#include <QValidator>
#include <QLineEdit>

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

    app_settings = settings.loadAppSettings();
    this->struct_settings2widgets();

    connect(settings_ui->pushButtonSave, &QPushButton::clicked,
            this, &SettingsForm::onSaveSettings);
    connect(settings_ui->pushButtonAdd, &QPushButton::clicked,
            this, &SettingsForm::onKeyAdd);
    connect(settings_ui->pushButtonDel, &QPushButton::clicked,
            this, &SettingsForm::onDeleteItem);
    connect(settings_ui->listWidget, &QListWidget::currentRowChanged,
            this, &SettingsForm::onItemChanged);
    connect(settings_ui->lineEditKey, &QLineEdit::textChanged,
            this, &SettingsForm::onLineEditKeytextChanged);

    QValidator *validator = new QRegExpValidator(QRegExp(KEY_RX), this);
    settings_ui->lineEditKey->setValidator(validator);
}

SettingsForm::~SettingsForm()
{
    delete settings_ui;
}


void SettingsForm::onSaveSettings()
{   
    app_settings.used = settings_ui->checkBoxProxyUsed->isChecked();
    app_settings.type = myproxytypes.type[settings_ui->comboBoxProxyType->currentIndex()];
    app_settings.host = settings_ui->lineEditProxyIP->text();
    app_settings.port = settings_ui->spinBoxProxyPort->value();
    app_settings.username = settings_ui->lineEditUser->text();
    app_settings.password = settings_ui->lineEditPassword->text();

    app_settings.app_keys = QStringList();
    for (int i = 0; i < settings_ui->listWidget->count(); ++i) {
        app_settings.app_keys.append(settings_ui->listWidget->item(i)->text());
    }
    
    int ret = settings.saveAppSettings(app_settings);
    
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
    settings_ui->checkBoxProxyUsed->setChecked(app_settings.used);
    settings_ui->lineEditProxyIP->setText(app_settings.host);
    settings_ui->comboBoxProxyType->setCurrentIndex(
        myproxytypes.type.indexOf(app_settings.type)
    );
    settings_ui->spinBoxProxyPort->setValue(app_settings.port);
    settings_ui->lineEditUser->setText(app_settings.username);
    settings_ui->lineEditPassword->setText(app_settings.password);

    for (int i = 0; i < app_settings.app_keys.size(); ++i) {
        settings_ui->listWidget->addItem(app_settings.app_keys.value(i));
    }
}


void SettingsForm::onKeyAdd()
{
    QString text = settings_ui->lineEditKey->text();
    if (Settings::is_key(text)) {
        settings_ui->listWidget->addItem(text);
        settings_ui->lineEditKey->clear();
    } else {
        qDebug() << "Nothing to add. It`s not key!";
    }
}

void SettingsForm::onDeleteItem()
{
    int row = settings_ui->listWidget->currentRow();
    if (row >= 0) {
        settings_ui->listWidget->takeItem(row);
    } else {
        qDebug() << "no rows";
    }
}


void SettingsForm::onItemChanged(int row)
{
    if (row >= 0) {
        settings_ui->pushButtonDel->setEnabled(true);
    } else {
        settings_ui->pushButtonDel->setEnabled(false);
    }
}


bool SettingsForm::_is_key_in_list(QString text)
{
    bool ret = false;
    for (int i = 0; i < settings_ui->listWidget->count(); ++i) {
        QListWidgetItem* item = settings_ui->listWidget->item(i);
        if (item->text() == text) {
            ret = true;
            break;
        }
    }
    return ret;
}

void SettingsForm::onLineEditKeytextChanged(QString text)
{
    if (Settings::is_key(text) && !_is_key_in_list(text)) {
        settings_ui->pushButtonAdd->setEnabled(true);
    } else {
        settings_ui->pushButtonAdd->setEnabled(false);
    }
}

int SettingsForm::key_count() {
    return app_settings.app_keys.size();
}
