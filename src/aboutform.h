#ifndef ABOUTFORM_H
#define ABOUTFORM_H

#include <QWidget>

namespace Ui
{
class AboutForm;
}


class AboutForm : public QWidget
{
    Q_OBJECT

public:

    AboutForm();
    ~AboutForm();


private:
    Ui::AboutForm* about_ui;
};

#endif // ABOUTFORM_H
