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
#include <QLineEdit>


#include "ui_mainform.h"
#include "mainform.h"



MainForm::MainForm(QWidget *parent)
    : QWidget(parent), ui(new Ui::Form)
{
    ui->setupUi(this);
    connect(ui->pushButtonGo, &QPushButton::clicked, this, &MainForm::getData);
    connect(ui->toolButtonInput, &QToolButton::clicked, this, &MainForm::on_setFileName);
    connect(ui->lineEdit, &QLineEdit::textChanged, this, &MainForm::on_go_ready);
}

MainForm::~MainForm()
{
    delete ui;
}

void MainForm::getData()
{
    ui->pushButtonStop->setEnabled(true);
    ui->pushButtonGo->setEnabled(false);
    if (ui->lineEdit->text() != "") {
        qDebug() << "parent: " << QThread::currentThread();
        MultiDownloader *task = new MultiDownloader(ui->lineEdit->text(),
                                                    ui->comboBoxSpeaker->currentText());
        connect(task, &MultiDownloader::on_progress_change,
                ui->progressBar, &QProgressBar::setValue);
        connect(task, &MultiDownloader::on_all_done,
                this, &MainForm::onReadingFinished);
        connect(task, &MultiDownloader::ready_voiced,
                this, &MainForm::onReady);
        connect(ui->pushButtonStop, &QPushButton::clicked,
                task, &MultiDownloader::cancel);
        connect(task, &MultiDownloader::need_abort,
                this, &MainForm::canceled);
        //QThreadPool::globalInstance()->start(task);
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
    if (!str.isEmpty()) ui->lineEdit->setText(str);
}

void MainForm::onReady(int max)
{
    ui->progressBar->setMaximum(max);
}


void MainForm::onReadingFinished(int err_code, int err_files, QString outfile_name)
{
    this->canceled();
    
    QMessageBox msgBox;
    QString msg = "";
    
    switch ( err_code ) {
        case MultiDownloader::ok: {
            msg.append(QString::fromUtf8("Все готово!"));
            break;
        }
        case MultiDownloader::err_network: {
            msg.append(QString::fromUtf8("Ошибка сети!"));
            break;
        } 
        case MultiDownloader::err_redirect: {
            msg.append(QString::fromUtf8("Ошибка сети!"));
            break;
        }
        case MultiDownloader::err_read_file: {
            msg.append(QString::fromUtf8("Ошибка! Не удается прочитать файл!"));
            break;
        }
        case MultiDownloader::err_write_file: {
            msg.append(QString::fromUtf8("Ошибка! Не удается записать в выходной файла!"));
            break;
        }
        case MultiDownloader::err_unsupported_mime_input_file: {
            msg.append(QString::fromUtf8("Ошибка! Не поддерживаемый тип файла!"));
            break;
        }
        case MultiDownloader::err_write_with_errors: {
        }
        case MultiDownloader::err_unsupported_encoding_input_file: {
            msg.append(QString::fromUtf8("Ошибка! Не поддерживаемая кодировка текстового файла!"));
            break;
        }
        case MultiDownloader::warn_not_voiced: {
            msg.append("\n");
            msg.append(QString::fromUtf8("Не удалось обработать %1 фрагментов.").arg(err_files));
            break;
        }
        case MultiDownloader::err_cancel: {
            break;
        }
    }
    if (!msg.isEmpty()) {
        msg.append("\n");
        msg.append(QString::fromUtf8("Выходной файл тут: %1").arg(outfile_name));
        msgBox.setText(msg);
        msgBox.exec();
    }
    
    qDebug() << "all_done" << ": " << err_code << ": " << err_files <<": " << outfile_name;
}

void MainForm::canceled()
{
    ui->pushButtonStop->setEnabled(false);
    ui->pushButtonGo->setEnabled(false);
    ui->progressBar->setValue(0);
    ui->lineEdit->setText("");
}

void MainForm::on_go_ready()
{
    ui->pushButtonStop->setEnabled(false);
    ui->pushButtonGo->setEnabled(true);
}

