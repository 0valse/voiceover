#ifndef MULTIDOWNLOADER_H
#define MULTIDOWNLOADER_H

#include <QObject>
#include <QString>
#include <QRunnable>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QFile>
#include <QHash>
#include <QMap>
#include <QStringList>
#include <QAtomicInt>


// consts
const int MAX_FILE_SIZE = 1 * 1024 * 1025;
const int MAX_TEXT_URL = 2000;

const QString KEY = "a3a66d18-e9d2-43af-9408-36a30416baed"; // TODO: use Qlist later
const QString URL_TEMPLATE = "https://tts.voicetech.yandex.net/generate?text=\"%2\"&format=%3&lang=ru-RU&speaker=%4&emotion=good&speed=%5&key=%1";
//const QString URL_TEMPLATE ="http://200ok-debian.rd.ptsecurity.ru:8000/get?text=\"%2\"&format=%3&lang=ru-RU&speaker=%4&emotion=good&speed=%5&key=%1";
const QString OUT_FORMAT = "mp3";
const QString VALID_MIME = "text/plain"; // TODO: use Qlist later

#define getCounter "getCounter"


const QList<QString> UA = QList<QString>()
    << "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36"
    << "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.71 Safari/537.36"
    << "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_6) AppleWebKit/602.1.50 (KHTML, like Gecko) Version/10.0 Safari/602.1.50"
    << "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.11; rv:49.0) Gecko/20100101 Firefox/49.0"
    << "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36"
    << "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.71 Safari/537.36"
    << "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.71 Safari/537.36"
    << "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_1) AppleWebKit/602.2.14 (KHTML, like Gecko) Version/10.0.1 Safari/602.2.14"
    << "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12) AppleWebKit/602.1.50 (KHTML, like Gecko) Version/10.0 Safari/602.1.50"
    << "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.79 Safari/537.36 Edge/14.14393"
    << "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36"
    << "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.71 Safari/537.36"
    << "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36"
    << "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.71 Safari/537.36"
    << "Mozilla/5.0 (Windows NT 10.0; WOW64; rv:49.0) Gecko/20100101 Firefox/49.0"
    << "Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36"
    << "Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.71 Safari/537.36"
    << "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36"
    << "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.71 Safari/537.36"
    << "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:49.0) Gecko/20100101 Firefox/49.0"
    << "Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko"
    << "Mozilla/5.0 (Windows NT 6.3; rv:36.0) Gecko/20100101 Firefox/36.0"
    << "Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36"
    << "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36"
    << "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:49.0) Gecko/20100101 Firefox/49.0";
// END const


class MultiDownloader : public QObject
{
    Q_OBJECT
public:
    enum ERRORS
    {
        ok,
        err_network,
        err_redirect,
        err_read_file,
        err_write_file,
        err_unsupported_mime_input_file,
        err_unsupported_encoding_input_file,
        err_cancel,
        warn_not_voiced,
        err_key_error
    };
    const int max_redirects = 5;  
    MultiDownloader(QString in_file_name, QString _speaker, QObject *parent = 0);
    ~MultiDownloader();
    static QString prepare_out_file_name(QString in_file_name);
    
signals:
    void on_progress_change(int now);
    void on_all_done(int err_code, int err_files, QString outfile_name);
    void ready_voiced(int max);
    void need_abort();

public slots:
    void cancel();
    void run();
    void on_one_read(QNetworkReply *reply);

private:
    bool key_err = false;
    QString speaker;
    float speed = 0.9;
    QFile *in_file = new QFile();
    QFile *out_file = new QFile();
    
    QNetworkAccessManager *manager;

    QHash<int, QUrl> in_list;
    QMap<int, QByteArray> out_list;
    QList<QUrl> err_texts;
    
    QAtomicInt m_cancelledMarker;

    void _prepare_list(QFile input);
    void _text2urls();
    void _clean();
};

#endif // MULTIDOWNLOADER_H