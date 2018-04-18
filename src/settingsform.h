#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QWidget>

namespace Ui
{
class SettingsForm;
}

/**
 * @todo write docs
 */
class SettingsForm : public QWidget
{
    Q_OBJECT

public:
    SettingsForm();
    ~SettingsForm();

private:
    Ui::SettingsForm* ui;
};

#endif // SETTINGSFORM_H
