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

#include <QWidget>
#include <QPushButton>
#include <QToolButton>
#include <QThreadPool>
#include <QProgressBar>
#include <QComboBox>
#include <QFileDialog>
#include <QtCore>
#include <QDir>
#include <QMessageBox>


#include "ui_mainform.h"
#include "mainform.h"



MainForm::MainForm(QWidget *parent)
    : QWidget(parent), ui(new Ui::Form)
{
    ui->setupUi(this);
    connect(ui->pushButtonGo, &QPushButton::clicked, this, &MainForm::getData);
    connect(ui->toolButtonInput, &QToolButton::clicked, this, &MainForm::on_setFileName);
    
}

MainForm::~MainForm()
{
    delete ui;
}

void MainForm::getData()
{
    if (ui->labelInput->text() != "") {
        qDebug() << "parent: " << QThread::currentThread();
        MultiDownloader *task = new MultiDownloader(ui->labelInput->text(),
                                                    ui->comboBoxSpeaker->currentText());
        //task->setParent(this);
        connect(task, &MultiDownloader::on_progress_change,
                ui->progressBar, &QProgressBar::setValue);
        connect(task, &MultiDownloader::on_all_done,
                this, &MainForm::onReadingFinished);
        connect(task, &MultiDownloader::ready_voiced,
                this, &MainForm::onReady);
        //QThreadPool *pool = new QThreadPool(); //globalInstance()->start(task);
        //pool->start(task);
        task->run();
    } else {
        QMessageBox msgBox;
        msgBox.setText("Файл не выбран!");
        msgBox.exec();
    }
}

void MainForm::on_setFileName() {
    QString str = QFileDialog::getOpenFileName(this,
                                               "Выбрать текстовый файл",
                                               QDir::currentPath(),
                                               "Text (*.txt)");
    ui->labelInput->setText(str);
}

void MainForm::onReady(int max)
{
    ui->progressBar->setMaximum(max);
}


void MainForm::onReadingFinished(int err_code, int err_files, QString outfile_name)
{
    QMessageBox msgBox;
    QString msg = "";
    
    switch ( err_code ) {
        case ok: {
            msg.append(QString::fromUtf8("Все готово!"));
            break;
        }
        case err_network: {
            msg.append(QString::fromUtf8("Ошибка сети!"));
            break;
        } 
        case err_redirect: {
            msg.append(QString::fromUtf8("Ошибка сети!"));
            break;
        }
        case err_read_file: {
            msg.append(QString::fromUtf8("Ошибка! Не удается прочитать файл!"));
            break;
        }
        case err_write_file: {
            msg.append(QString::fromUtf8("Ошибка! Не удается записать в выходной файла!"));
            break;
        }
        case err_unsupported_mime_input_file: {
            msg.append(QString::fromUtf8("Ошибка! Не поддерживаемый тип файла!"));
            break;
        }
        case err_write_with_errors: {
        }
        case err_unsupported_encoding_input_file: {
            msg.append(QString::fromUtf8("Ошибка! Не поддерживаемая кодировка текстового файла!"));
            break;
        }
        case warn_not_voiced: {
            msg.append("\n");
            msg.append(QString::fromUtf8("Не удалось обработать %1 фрагментов.").arg(err_files));
            break;
        }
    }
    msg.append("\n");
    msg.append(QString::fromUtf8("Выходной файл тут: %1").arg(outfile_name));
    msgBox.setText(msg);
    msgBox.exec();
    
    qDebug() << "all_done" << ": " << err_code << ": " << err_files <<": " << outfile_name;
}

