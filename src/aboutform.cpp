#include "aboutform.h"
#include "ui_aboutform.h"

AboutForm::AboutForm()
{
    about_ui = new Ui::AboutForm;
    about_ui->setupUi(this);
}

AboutForm::~AboutForm()
{
    delete about_ui;
}
