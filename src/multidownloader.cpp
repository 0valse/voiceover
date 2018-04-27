#include <QIODevice>
#include <QList>
#include <QMimeDatabase>
#include <QMimeType>
#include <QTextCodec>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QObject>
#include <QHashIterator>
#include <QRandomGenerator>


#include <uchardet.h>

#include "multidownloader.h"
#include "settings.h"


MultiDownloader::MultiDownloader(QString in_file_name, QString _speaker, QObject *parent)
    : QObject(parent), speaker(_speaker), m_cancelledMarker(false)
{
    manager = new QNetworkAccessManager(this);

    connect(manager, &QNetworkAccessManager::finished, this, &MultiDownloader::on_one_read);
    
    in_file->setFileName(in_file_name);
    out_file->setFileName(MultiDownloader::prepare_out_file_name(in_file_name));
    
    Settings settings;
    ProxySettings proxy_settings = settings.loadProxySettings();
    
    QNetworkProxy proxy;
    if (proxy_settings.used) {
        proxy.setType((QNetworkProxy::ProxyType)proxy_settings.type);
        proxy.setHostName(proxy_settings.host);
        proxy.setPort(proxy_settings.port);

        if (!(proxy_settings.username.isEmpty() or proxy_settings.password.isEmpty())) {
            proxy.setUser(proxy_settings.username);
            proxy.setPassword(proxy_settings.password);
        }
    } else { // if (proxy_settings.used) {
        proxy.setType(QNetworkProxy::NoProxy);
    }
    qDebug() << proxy;
    manager->setProxy(proxy);
}

MultiDownloader::~MultiDownloader()
{
    _clean();
    if (!in_file->isOpen()) {
        in_file->close();
    }
    if (!out_file->isOpen()) {
        out_file->close();
    }
    delete in_file;
    delete out_file;
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
    if( m_cancelledMarker.testAndSetAcquire( true, true ) ) {
        emit on_all_done(MultiDownloader::err_cancel, 0, "");
        return;
    }
    
    _clean();
    _text2urls();
    if (!out_file->open(QIODevice::WriteOnly)) {
        emit on_all_done(MultiDownloader::err_write_file, 0, out_file->fileName());
    }
    
    QNetworkReply *rpl;
    int rand_num = QRandomGenerator::global()->bounded(0, UA.size());
    for (int i = 0; i < in_list.size(); ++i) {
        QNetworkRequest r;
        r.setUrl(in_list[i]);
        r.setHeader(QNetworkRequest::UserAgentHeader, UA[rand_num]);
        
        rpl = manager->get(r);
        
        rpl->setProperty(getCounter, QVariant(i));
        connect(this, &MultiDownloader::need_abort, rpl, &QNetworkReply::abort);
    }
}

void MultiDownloader::_clean()
{
    out_list.clear();
    in_list.clear();
    err_texts.clear();
}


void MultiDownloader::_text2urls() {
    if (!in_file->open(QIODevice::ReadOnly)) {
        emit on_all_done(MultiDownloader::err_read_file, 0, "");
        return;
    }
    
    QByteArray buf = in_file->read(MAX_FILE_SIZE);
    in_file->close();

    QMimeType mtype = QMimeDatabase().mimeTypeForData(buf);
    qDebug() << "File MIME type: " << mtype.name();
    
    if (mtype.name() != VALID_MIME ) {
        emit on_all_done(MultiDownloader::err_unsupported_mime_input_file, 0, "");
        return;
    }
    
    uchardet_t upage = uchardet_new();
    int retval = uchardet_handle_data(upage, buf.constData(), buf.size());
    if (retval != 0) {
        emit on_all_done(MultiDownloader::err_unsupported_encoding_input_file, 0, "");
        uchardet_data_end(upage);
        uchardet_delete(upage);
        return;
    }
    uchardet_data_end(upage);
    const char *encode_page;
    encode_page = strdup(uchardet_get_charset(upage));
    uchardet_delete(upage);
    
    qDebug() << "File codec: " << encode_page;
    QTextCodec *codec = QTextCodec::codecForName(encode_page);

    encode_page = nullptr;
    delete encode_page;
    
    QString s = codec->toUnicode(buf.constData());
    buf.clear();
    
    s = s.trimmed().replace(".\n", ". ").replace("\n", ". ").simplified();
    s = s.replace("   ", " ").replace("  ", " ").replace("..", ".");
    s = s.replace(";.", ";").replace(":.", ":").replace("?.", "?").replace("!.", "!").replace("***", "\n");
    
    QStringList slines = s.split(". ");
    
    QString text = "";
    QString _tmp_txt;
    int count = 0;
    int rand_num = 0;

    for (int i = 0; i < slines.size(); ++i) {
      _tmp_txt = slines[i].trimmed();
      
      if (QUrl::toPercentEncoding(text).size() + 
          QUrl::toPercentEncoding(_tmp_txt).size() + 1 
          < MAX_TEXT_URL) {
           text.append(_tmp_txt).append(". ");
      } else {
         in_list[count] = QUrl(
             URL_TEMPLATE.arg(KEYS[rand_num], text.trimmed(),
                              OUT_FORMAT, speaker, QString::number(speed))
         );
        if (rand_num == KEYS.size()) {
             rand_num = 0;
        } else {
            ++rand_num;
        }
         ++count;
         
         qDebug() << "Text: " << text;

         text.clear();
         text.append(_tmp_txt).append(". ");
      }
    }
    // add last
    if (!text.isEmpty()) {
        in_list[count] = QUrl(
             URL_TEMPLATE.arg(KEYS[rand_num], text.trimmed(),
                              OUT_FORMAT, speaker, QString::number(speed))
        );
        if (rand_num == KEYS.size()) {
             rand_num = 0;
        } else {
            ++rand_num;
        }
        qDebug() << "Last Text: " << text;
    }

    slines.clear();
    emit ready_voiced(in_list.size());
}


void MultiDownloader::on_one_read(QNetworkReply* reply)
{
    int in_size = in_list.size();
    int out_size = out_list.size() + 1;
    int code;
    int ret_code;

    QVariant var = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (var.canConvert<int>()) {
        ret_code = var.toInt();
    } else {
        ret_code = 0;
    }
    
    qDebug() << "Retcode: " << ret_code;
    
    if(m_cancelledMarker.testAndSetAcquire(true, true)) {
        if (out_file->isOpen())
            out_file->close();
        reply->abort();
        return;
    } else {
        emit on_progress_change(out_size);
    }

    QNetworkRequest r = reply->request();
    QVariant prop = reply->property(getCounter);
    int i;
    if (prop.canConvert<int>()) {
        i = prop.toInt();
    }
    else {
        i = 0;
    }

    if(reply->error()) {
        // key problems: ret code 423
        if (ret_code == 423) key_err = true;

        qDebug() << "ERROR";
        qDebug() << reply->errorString();

        err_texts.append(r.url());

        // запишем пустой массив
        out_list[i] = QByteArray();

    } else { // if(reply->error())
        out_list[i] = reply->readAll();
    }

    if (in_size == out_size) {
        if (key_err) {
            emit on_all_done(MultiDownloader::err_key_error, 0, "");
        } else {
            QMapIterator<int, QByteArray> i(out_list);
            while (i.hasNext()) {
                i.next();
                out_file->write(i.value());
            }
            out_file->close();
            
            if (err_texts.isEmpty()) {
                code = MultiDownloader::ok;
            } else {
                code = MultiDownloader::warn_not_voiced;
            }
            emit on_all_done(code, err_texts.size(), out_file->fileName());
        }
    }

    reply->deleteLater();
    
}

void MultiDownloader::cancel() {
    m_cancelledMarker.fetchAndStoreAcquire(true);
    disconnect(manager, &QNetworkAccessManager::finished, 0, 0);
    emit need_abort();
    emit on_all_done(MultiDownloader::err_cancel, 0, "");
}
