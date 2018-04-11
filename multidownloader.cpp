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

#include <QIODevice>
#include <QList>
#include <QMimeDatabase>
#include <QMimeType>
#include <QTextCodec>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QObject>
#include <QHashIterator>
#include <QRandomGenerator>

#include <QThread>

#include <uchardet/uchardet.h>

#include "multidownloader.h"


MultiDownloader::MultiDownloader(QString in_file_name, QString _speaker, QObject *parent)
    : QObject(parent), QRunnable(), speaker(_speaker), m_cancelledMarker(false)
{
    in_file->setFileName(in_file_name);
    out_file->setFileName(MultiDownloader::prepare_out_file_name(in_file_name));
}

MultiDownloader::~MultiDownloader()
{
    _clean();
}

QString MultiDownloader::prepare_out_file_name(QString in_file_name) {
    QStringList lst = in_file_name.split(".");
    QString out;
    
    lst.removeLast();
    lst << OUT_FORMAT;

    out = lst.join('.');
    return out;
}


void MultiDownloader::run()
{
    qDebug() << "in thread: " << QThread::currentThread();

    if( m_cancelledMarker.testAndSetAcquire( true, true ) ) {
        emit on_all_done(err_cancel, 0, "");
        return;
    }
    
    _clean();
    _text2urls();
    if (!out_file->open(QIODevice::WriteOnly)) {
        emit on_all_done(err_write_file, 0, out_file->fileName());
    }

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &MultiDownloader::on_one_read);

    for (int i = 0; i < in_list.size(); ++i) {
        QNetworkRequest r;
        r.setUrl(in_list[i]);
        r.setAttribute(QNetworkRequest::User, QVariant(i));
        int rand_num = QRandomGenerator::global()->bounded(0, UA.size());
        r.setHeader(QNetworkRequest::UserAgentHeader, UA[rand_num]);
        manager->get(r);
    }
}

void MultiDownloader::_clean()
{
    out_list.clear();
    in_list.clear();
    /*if (!in_file->isOpen()) {
        in_file->close();
    }
    if (!out_file->isOpen()) {
        out_file->close();
    }
    */
}


void MultiDownloader::_text2urls() {
    if (!in_file->open(QIODevice::ReadOnly)) {
        emit on_all_done(err_read_file, 0, "");
        return;
    }
    
    QByteArray buf = in_file->read(MAX_FILE_SIZE);
    in_file->close();

    QMimeType mtype = QMimeDatabase().mimeTypeForData(buf);
    qDebug() << mtype.name();
    
    if (mtype.name() != VALID_MIME ) {
        emit on_all_done(err_unsupported_mime_input_file, 0, "");
        return;
    }
    
    uchardet_t upage = uchardet_new();
    int retval = uchardet_handle_data(upage, buf.constData(), buf.size());
    if (retval != 0) {
        emit on_all_done(err_unsupported_encoding_input_file, 0, "");
        uchardet_data_end(upage);
        uchardet_delete(upage);
        return;
    }
    uchardet_data_end(upage);
    const char *encode_page = strdup(uchardet_get_charset(upage));
    uchardet_delete(upage);
    
    qDebug() << encode_page;
    QTextCodec *codec = QTextCodec::codecForName(encode_page);

    encode_page = nullptr;
    delete encode_page;
    
    QString s = codec->toUnicode(buf.constData());
    buf.clear();
    
    s = s.trimmed().replace(".\n", ". ").replace("\n", ". ").simplified();
    s.replace("   ", " ").replace("  ", " ").replace("..", ".");
    qDebug() << s;

    QStringList slines = s.split(". ");
    
    QString text = "";
    QString _tmp_txt;
    int count = 0;
    for (int i = 0; i < slines.size(); ++i) {
      _tmp_txt = slines[i].trimmed();
      if (QUrl::toPercentEncoding(text).size() + QUrl::toPercentEncoding(_tmp_txt).size() + 1 < MAX_TEXT_URL) {
           text.append(_tmp_txt).append(". ");
      } else {
         in_list[count] = QUrl(
             URL_TEMPLATE.arg(KEY, text.trimmed(), OUT_FORMAT, speaker, QString::number(speed))
         );
         ++count;
         text.clear();
      }
    }
    // add last
    if (!text.isEmpty())
        in_list[count] = QUrl(
             URL_TEMPLATE.arg(KEY, text.trimmed(), OUT_FORMAT, speaker, QString::number(speed))
         );
    slines.clear();
    emit ready_voiced(in_list.size());
}


void MultiDownloader::on_one_read(QNetworkReply* reply)
{
    if(m_cancelledMarker.testAndSetAcquire(true, true)) {
        emit on_all_done(err_cancel, 0, "");
        return;
    }

    QNetworkRequest r = reply->request();
    int i = r.attribute(QNetworkRequest::User, 0).toInt();
    
    emit on_progress_change(out_list.size()+1);

    if(reply->error()) {

        qDebug() << "ERROR";
        qDebug() << reply->errorString();
        err_texts.append(r.url());
        // запишем пустой массив
        out_list[i] = QByteArray();

    } else { // if(reply->error())

        out_list[i] = reply->readAll();
        int in_size = in_list.size();
        int out_size = out_list.size();

        if (in_size == out_size) {
            QMapIterator<int, QByteArray> i(out_list);
            while (i.hasNext()) {
                i.next();
                out_file->write(i.value());
            }
            out_file->close();

            int code;
            if (err_texts.isEmpty()) {
                code = ok;
            } else {
                code = warn_not_voiced;
            }
            emit on_all_done(code, err_texts.size(), out_file->fileName());

        } //if (in_size == out_size)

    }
    reply->deleteLater();
}

void MultiDownloader::cancel() {
    m_cancelledMarker.fetchAndStoreAcquire( true );
}
