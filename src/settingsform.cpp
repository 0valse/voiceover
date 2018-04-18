#include "settingsform.h"
#include "ui_settingsform.h"

SettingsForm::SettingsForm()
{
    ui = new Ui::SettingsForm;
    ui->setupUi(this);
}

SettingsForm::~SettingsForm()
{
    delete ui;
}
