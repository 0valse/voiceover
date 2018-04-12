/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2018  Alexandr Ovsyannikov <aovsyannikov@ptsecurity.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MAINFORM_H
#define MAINFORM_H

#include <QWidget>

#include "multidownloader.h"
#include "ui_mainform.h"

namespace Ui {
class MainForm;
}

class MainForm : public QWidget
{
    Q_OBJECT

public:
    explicit MainForm(QWidget *parent = 0);
    ~MainForm();

private slots:
    void onReady(int max);
    void onReadingFinished(int err_code, int err_files, QString outfile_name);
    void on_setFileName();
    void getData();
    void canceled();
    void on_go_ready();
    void play_toggle();

private:
    Ui::Form* ui;
    
};

#endif // MAINFORM_H
