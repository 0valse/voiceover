#include "settingsform.h"
#include "ui_settingsform.h"

SettingsForm::SettingsForm()
{
    settings_ui = new Ui::SettingsForm;
    settings_ui->setupUi(this);
}

SettingsForm::~SettingsForm()
{
    delete settings_ui;
}
