#include "aboutform.h"
#include "ui_aboutform.h"

AboutForm::AboutForm()
{
    ui = new Ui::AboutForm;
    ui->setupUi(this);
}

AboutForm::~AboutForm()
{
    delete ui;
}
