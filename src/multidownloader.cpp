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

    connect(manager, &QNetworkAccessManager::finished, this, &MultiDownloader::onServerGetData);
    
    in_file->setFileName(in_file_name);
    out_file->setFileName(MultiDownloader::prepare_out_file_name(in_file_name));
    
    Settings settings;
    _appSettings app_settings = settings.loadAppSettings();

    // загрузим из конфига массив ключей
    KEYS = app_settings.app_keys;
    
    // включим использование прокси (из настроек) на всю сессию озвучки
    QNetworkProxy proxy;
    if (app_settings.used) {
        proxy.setType((QNetworkProxy::ProxyType)app_settings.type);
        proxy.setHostName(app_settings.host);
        proxy.setPort(app_settings.port);

        if (!(app_settings.username.isEmpty() or app_settings.password.isEmpty())) {
            proxy.setUser(app_settings.username);
            proxy.setPassword(app_settings.password);
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

// выполнение одиночного запроса к серверу озвучки
void MultiDownloader::_reqOne(int text_id, int key_id)
{
    QUrl url = QUrl();
    url.setUrl(URL_TEMPLATE.arg(
                   KEYS.value(key_id), in_list.value(text_id),
                    OUT_FORMAT, speaker, QString::number(speed)
                   )
               );

    int rand_num = QRandomGenerator::global()->bounded(0, UA.size());

    QNetworkRequest r(url);
    r.setHeader(QNetworkRequest::UserAgentHeader, UA[rand_num]);

    QNetworkReply *rpl;
    rpl = manager->get(r);
    rpl->setProperty(getCounter, QVariant(text_id));
    connect(this, &MultiDownloader::need_abort, rpl, &QNetworkReply::abort);
}

void MultiDownloader::run()
{
    active_key_num = 0;

    if (KEYS.size() < 1) {
        all_done(MultiDownloader::err_no_keys, 0, "");
        return;
    }

    if( m_cancelledMarker.testAndSetAcquire( true, true ) ) {
        all_done(MultiDownloader::err_cancel, 0, "");
        return;
    }
    
    _clean();
    _text2list();
    if (!out_file->open(QIODevice::WriteOnly)) {
        all_done(MultiDownloader::err_write_file, 0, out_file->fileName());
    }
    
    for (int i = 0; i < in_list.size(); ++i) {
        _reqOne(i, active_key_num);
    }
}

void MultiDownloader::_clean()
{
    out_list.clear();
    in_list.clear();
    err_texts.clear();
}

// метод подготовки массива URL
void MultiDownloader::_text2list() {
    if (!in_file->open(QIODevice::ReadOnly)) {
        all_done(MultiDownloader::err_read_file, 0, "");
        return;
    }
    
    QByteArray buf = in_file->read(MAX_FILE_SIZE);
    in_file->close();

    // Определим MIME тип входных данных
    QMimeType mtype = QMimeDatabase().mimeTypeForData(buf);
    qDebug() << "File MIME type: " << mtype.name();
    
    // для невалидных типов эмитим ошибку
    // пока поддерживаем только TXT
    if (mtype.name() != VALID_MIME ) {
        emit all_done(MultiDownloader::err_unsupported_mime_input_file, 0, "");
        return;
    }
    
    // определим кодировку текстового файла
    uchardet_t upage = uchardet_new();
    int retval = uchardet_handle_data(upage, buf.constData(), buf.size());
    if (retval != 0) {
        all_done(MultiDownloader::err_unsupported_encoding_input_file, 0, "");
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
    
    // конвертим входной текст в Unicode из определенной выше кодировки
    QString s = codec->toUnicode(buf.constData());
    buf.clear();
    
    // тупая обработка лишних символов: образаем лишние проблемы, убирем символы переноса строки, перевода каретки,..
    s = s.trimmed().replace(".\n", ". ").replace("\n", ". ").simplified();
    s = s.replace("   ", " ").replace("  ", " ").replace("..", ".");
    s = s.replace(";.", ";").replace(":.", ":").replace("?.", "?").replace("!.", "!").replace("***", "\n");
    
    // сплитим в массив по предложениям
    QStringList slines = s.split(". ");
    
    QString text = "";
    QString _tmp_txt;
    int count = 0;
    int key_num = 0;
    const int max_key_num = KEYS.size();

    // заполним массив URLами чтобы размер полезного текста был не более 2000 (urlencoded)
    // элементы массива заполняются только предложениями.
    for (int i = 0; i < slines.size(); ++i) {
      _tmp_txt = slines[i].trimmed();
      
      if (QUrl::toPercentEncoding(text).size() + 
          QUrl::toPercentEncoding(_tmp_txt).size() + 1 
          < MAX_TEXT_URL) {
           text.append(_tmp_txt).append(". ");
      } else {
          if (key_num == max_key_num)
               key_num = 0;

          in_list[count] = text.trimmed();
          ++key_num;
          ++count;

         text.clear();
         text.append(_tmp_txt).append(". ");
      }
    }

    // что осталось
    if (!text.isEmpty()) {

        if (key_num == KEYS.size())
             key_num = 0;

        in_list[count] = text.trimmed();
    }

    slines.clear();
    emit ready_voiced(in_list.size());
}


// слот обработки ответа от сервера озвучки
void MultiDownloader::onServerGetData(QNetworkReply* reply)
{
    int in_size = in_list.size();
    int out_size = out_list.size() + 1;
    int code, ret_code, fragment;
    bool code_ok, counter_ok;

    // если ранее был получен сигнал аборта
    if(m_cancelledMarker.testAndSetAcquire(true, true)) {
        // если ранее был получен сигнал аборта

        if (out_file->isOpen())
            out_file->close();
        reply->abort();
        return;
    } else {
        // увеличим прогресс наполнения аудио-массива
        emit on_progress_change(out_size);
    }

    QNetworkRequest r = reply->request();

    // получить номер аудио-фрагмента
    fragment = reply->property(getCounter).toInt(&counter_ok);
    if (!counter_ok) fragment = 0;

    qDebug() << "fragment: " << fragment << "key num: " << active_key_num;

    // обработать ответ сервера
    if(reply->error()) {
        // получить код ошибки сервера
        ret_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(&code_ok);
        if (!code_ok) ret_code = 0;

        qDebug() << "Error with retcode: " << ret_code;

        // key problems: ret code 423
        if (ret_code == 423) {
            // попробовать другой ключ. Если все включи исчерпались, то поднять ошибку
            if (active_key_num + 1 >= KEYS.size()) {
                key_err = true;
            } else {
               qDebug() << "key: " << active_key_num << " " << KEYS.value(active_key_num, "?");
               ++active_key_num;
               _reqOne(fragment, active_key_num);
               reply->deleteLater();
               return;
            }
        }

        qDebug() << "ERROR: " << reply->errorString();

        err_texts.append(r.url());

        // запишем пустой массив
        out_list[fragment] = QByteArray();

    } else { // if(reply->error())
        // нет ошибок - записать ответ сервера в аудио-массив
        out_list[fragment] = reply->readAll();
    }

    // оценить, что все загружено. Записать массив в файл и эмитить сигнал завершения
    if (in_size == out_size) {
        if (key_err) {
            all_done(MultiDownloader::err_key_error, 0, "");
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
            all_done(code, err_texts.size(), out_file->fileName());
        }
    }

    reply->deleteLater();  
}


void MultiDownloader::cancel() {
    m_cancelledMarker.fetchAndStoreAcquire(true);
    disconnect(manager, &QNetworkAccessManager::finished, 0, 0);
    emit need_abort();
    all_done(MultiDownloader::err_cancel, 0, "");
}


void MultiDownloader::all_done(int code, int err_text_size, QString out_file_name)
{
    emit on_all_done(code, err_text_size, out_file_name);
    this->_clean();
}
