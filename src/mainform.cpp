#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QToolButton>
#include <QProgressBar>
#include <QComboBox>
#include <QTextEdit>
#include <QFileDialog>
#include <QtCore>
#include <QDir>
#include <QMessageBox>
#include <QLineEdit>
#include <QFile>
#include <QFileInfo>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QAction>

#include "ui_mainform.h"

#include "mainform.h"
#include "settings.h"


MainForm::MainForm(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(APPLICATION_NAME);

    connect(ui->pushButtonGo, &QPushButton::clicked, this, &MainForm::getData);
    connect(ui->toolButtonInput, &QToolButton::clicked, this, &MainForm::on_setFileName);
    connect(ui->lineEdit, &QLineEdit::textChanged, this, &MainForm::on_go_ready);
    connect(ui->pushButtonPlay, &QPushButton::clicked, this, &MainForm::play_toggle);

    m_player = new QMediaPlayer(this);
    m_player->setVolume(100);

    plst = new QMediaPlaylist(m_player);
    
    m_player->setPlaylist(plst);
    plst->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    
    for (int i = 0; i < voicer.voicer_name.size(); ++i) {
        ui->comboBoxSpeaker->addItem(voicer.voicer_name[i]);
        plst->addMedia(QUrl(
                QString("qrc:/examples/%1").arg(voicer.voicer[i]))
        );
    }

    QObject::connect(ui->actionAbout, &QAction::triggered, this, &MainForm::show_about);
    QObject::connect(ui->actionSettings, &QAction::triggered, this, &MainForm::show_settings);
}

MainForm::~MainForm()
{
    delete ui;
}


void MainForm::show_about()
{
    about.show();
}

void MainForm::show_settings()
{
    settings.show();
}


void MainForm::getData()
{
    ui->pushButtonStop->setEnabled(true);
    ui->pushButtonGo->setEnabled(false);
    if (ui->lineEdit->text() != "") {
        task = new MultiDownloader(ui->lineEdit->text(),
                                voicer.voicer[ui->comboBoxSpeaker->currentIndex()],
                                this);
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
                                               QDir::homePath(),
                                               "Text (*.txt)");
    qDebug() << str;
    QString out_str = MultiDownloader::prepare_out_file_name(str);
    if (!str.isEmpty()) {
        QFileInfo check_file(out_str);
        QMessageBox::StandardButton reply;
        if (check_file.exists() && check_file.isFile()) {
            reply = QMessageBox::question(this,
                                    QString::fromUtf8("Заменить?"),
                                    QString::fromUtf8("Выходной файл сущесвует.\nЗаменить?"),
                                    QMessageBox::Yes|QMessageBox::No
            );
            if (reply == QMessageBox::Yes) {
                ui->lineEdit->setText(str);
                ui->lineEditAudio->setText(out_str);
            } else {
                this->canceled();
            }
        } //if (check_file.exists() && check_file.isFile())
    ui->lineEdit->setText(str);
    ui->lineEditAudio->setText(out_str);
    } 
}

void MainForm::onReady(int max)
{
    ui->progressBar->setMaximum(max);
}


void MainForm::onReadingFinished(int err_code, int err_files, QString outfile_name)
{
    this->canceled();
    
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Озвучка завершена");
    msgBox.setTextFormat(Qt::RichText); // this does the magic trick and allows you to click the link
    
    QString msg = "";
    
    switch (err_code) {
        case MultiDownloader::ok: {
            msg.append(
                QString::fromUtf8("Все готово!<br />") +
                QString::fromUtf8("Выходной файл тут: <a href=\"%1\">%1</a>").arg(outfile_name)
            );
            msgBox.setIcon(QMessageBox::Information);
            break;
        }
        case MultiDownloader::err_network: {
            msgBox.setIcon(QMessageBox::Critical);
            msg.append(QString::fromUtf8("Ошибка сети!"));
            break;
        } 
        case MultiDownloader::err_redirect: {
            msgBox.setIcon(QMessageBox::Critical);
            msg.append(QString::fromUtf8("Ошибка сети!"));
            break;
        }
        case MultiDownloader::err_read_file: {
            msgBox.setIcon(QMessageBox::Critical);
            msg.append(QString::fromUtf8("Ошибка! Не удается прочитать файл!"));
            break;
        }
        case MultiDownloader::err_write_file: {
            msgBox.setIcon(QMessageBox::Critical);
            msg.append(QString::fromUtf8("Ошибка! Не удается записать в выходной файла!"));
            break;
        }
        case MultiDownloader::err_unsupported_mime_input_file: {
            msgBox.setIcon(QMessageBox::Critical);
            msg.append(QString::fromUtf8("Ошибка! Не поддерживаемый тип файла!"));
            break;
        }
        case MultiDownloader::err_unsupported_encoding_input_file: {
            msgBox.setIcon(QMessageBox::Critical);
            msg.append(
                QString::fromUtf8("Ошибка! Не поддерживаемая кодировка текстового файла!")
            );
            break;
        }
        case MultiDownloader::warn_not_voiced: {
            msgBox.setIcon(QMessageBox::Warning);
            msg.append("Все готово!<br />");
            msg.append(QString::fromUtf8("Не удалось обработать %1 фрагмент(a/ов).").arg(err_files));
            break;
        }
        case MultiDownloader::err_cancel: {
            msgBox.setIcon(QMessageBox::Warning);
            msg.append("Озвучка прервана");
            break;
        }
        case MultiDownloader::err_key_error: {
            msgBox.setIcon(QMessageBox::Critical);
            msg.append("Ошибка доступа к серверу озвучки!");
            break;
        }
    }
    msgBox.setText(msg);
    msgBox.exec();
    qDebug() << "all_done" << ": " << err_code << ": " << err_files <<": " << outfile_name;
}

void MainForm::canceled()
{
    ui->pushButtonStop->setEnabled(false);
    ui->pushButtonGo->setEnabled(false);
    ui->progressBar->setValue(0);
    ui->lineEdit->setText("");
    ui->lineEditAudio->setText("");
}

void MainForm::on_go_ready()
{
   if (ui->lineEdit->text() != "") {
        ui->pushButtonStop->setEnabled(false);
        ui->pushButtonGo->setEnabled(true);
   }
}

void MainForm::play_toggle()
{
    int cur = ui->comboBoxSpeaker->currentIndex();
    if (cur == -1) cur = 0;
    
    QIcon icon1;
    icon1.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);

    qDebug() << m_player->state();
    
    switch (m_player->state()) {
        case  QMediaPlayer::PausedState:
        case  QMediaPlayer::StoppedState: {
            m_player->stop();
            plst->setCurrentIndex(cur);
            m_player->play();
            ui->pushButtonPlay->setText(QString::fromUtf8("Стоп"));
            
            QString iconThemeName = QStringLiteral("media-playback-stop");
            if (QIcon::hasThemeIcon(iconThemeName))
                icon1 = QIcon::fromTheme(iconThemeName);

            break;
        }
        case QMediaPlayer::PlayingState: {
            m_player->stop();
            ui->pushButtonPlay->setText(QString::fromUtf8("Тест"));
            
            QString iconThemeName = QStringLiteral("media-playback-start");
            if (QIcon::hasThemeIcon(iconThemeName))
                icon1 = QIcon::fromTheme(iconThemeName);
            
            break;
        }
    }
    
    ui->pushButtonPlay->setIcon(icon1);

    qDebug() << m_player->state();
}
