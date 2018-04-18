#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QWidget>

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

private:
    Ui::SettingsForm* settings_ui;
};

#endif // SETTINGSFORM_H
