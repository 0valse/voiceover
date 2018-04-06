#!/usr/bin/env python3

import signal
import sys
import argparse
from random import choice
import chardet
import re

from PyQt5.QtCore import (QCoreApplication, QObject, QRunnable, QFile, QIODevice,
                          QUrl, pyqtSignal, QByteArray, QThread, QThreadPool,
                          QMimeDatabase)
from PyQt5.QtNetwork import QNetworkRequest, QNetworkAccessManager, QNetworkReply




## VARS

KEY="a3a66d18-e9d2-43af-9408-36a30416baed"
#url = 'https://tts.voicetech.yandex.net/generate?text="{text}"&format={format}&lang=ru-RU&speaker={speaker}&emotion=good&speed={speed}&key=%s' % KEY
url = 'http://200ok-debian.rd.ptsecurity.ru:8000/get?text="{text}"&format={format}&lang=ru-RU&speaker={speaker}&emotion=good&speed={speed}'


headers = {
    'Accept': '*/*',
    'Accept-Encoding': 'gzip, deflate, br',
    'Accept-Language': 'ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7',
    "User-Agent": "Mozilla/5.0 (Windows NT 6.3; rv:36.0) Gecko/20100101 Firefox/36.0"
}

MAX_FILE_SIZE = 1 * 1024 * 1025
MAX_TEXT_URL = 2000

# for random UA
user_agents = [
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.71 Safari/537.36",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_6) AppleWebKit/602.1.50 (KHTML, like Gecko) Version/10.0 Safari/602.1.50",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.11; rv:49.0) Gecko/20100101 Firefox/49.0",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.71 Safari/537.36",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.71 Safari/537.36",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_1) AppleWebKit/602.2.14 (KHTML, like Gecko) Version/10.0.1 Safari/602.2.14",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12) AppleWebKit/602.1.50 (KHTML, like Gecko) Version/10.0 Safari/602.1.50",
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.79 Safari/537.36 Edge/14.14393"
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36",
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.71 Safari/537.36",
    "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36",
    "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.71 Safari/537.36",
    "Mozilla/5.0 (Windows NT 10.0; WOW64; rv:49.0) Gecko/20100101 Firefox/49.0",
    "Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36",
    "Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.71 Safari/537.36",
    "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36",
    "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.71 Safari/537.36",
    "Mozilla/5.0 (Windows NT 6.1; WOW64; rv:49.0) Gecko/20100101 Firefox/49.0",
    "Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko",
    "Mozilla/5.0 (Windows NT 6.3; rv:36.0) Gecko/20100101 Firefox/36.0",
    "Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36",
    "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36",
    "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:49.0) Gecko/20100101 Firefox/49.0",
]


## MAIN work

class Download(QObject):
    downloadFinished = pyqtSignal(int, int, QByteArray)
    max_redirects = 5
    errors = {
        "network": 1,
        "redirect": 2,
        "ok": 0,
        "cancel": 255
    }
    current_url = None

    def __init__(self, num, url, parent=None, redirected=False, ua=None):
        super(Download, self).__init__(parent)
        self.parent = parent
        self.request = QNetworkRequest()
        self.mgr = QNetworkAccessManager()
        self.mgr.finished.connect(self.on_download_finished)

        self.current_url = QUrl(url)
        self.request.setUrl(self.current_url)
        if ua is not None:
            self.request.setHeader(QNetworkRequest.UserAgentHeader,
                                   ua)
        self.num = num
        if not redirected:
            self.redirect_count = 0
        self.mgr.get(self.request)

    def on_download_finished(self, reply):
        if reply.error() != QNetworkReply.NoError:
            #
            print(reply.errorString(), file=sys.stderr)
            #
            self.downloadFinished.emit(self.errors["network"],
                                       self.num,
                                       QByteArray())
        else:
            redirect_url = reply.attribute(QNetworkRequest.RedirectionTargetAttribute)
            if redirect_url is not None:
                # защита от зацикливания бесконечной ссылкой на самого себя
                if redirect_url == self.current_url:
                    self.downloadFinished.emit(self.errors["redirect"],
                                               self.num,
                                               QByteArray())
                else:
                    self.current_url = self.current_url.resolved(redirect_url)
                    # защита от зацикливания редиректов по количеству
                    if self.redirect_count < self.max_redirects:
                        self.redirect_count += 1
                        self.__init__(self.num, self.current_url, parent=self.parent, redirected=True)
                    else:
                        self.downloadFinished.emit(self.errors["redirect"],
                                                   self.num,
                                                   QByteArray())
            else:  # redirect_url if empty
                buf = reply.readAll()
                self.downloadFinished.emit(self.errors["ok"],
                                           self.num,
                                           buf)
        reply.deleteLater()

class MultiDownloader(QObject, QRunnable):
    # прогресс: стало, всего
    progressChanged = pyqtSignal(int, int)
    # код ошибки, имя файла, количество ошибок
    all_done = pyqtSignal(int, str, int)
    max_buf_len = 0

    errors = {
        "network": 1,
        "redirect": 2,
        "ok": 0,
        "cancel": 255,
        "read_file": 3,
        "write_file": 4,
        "write_with_errors": 5,
        "unsupported_input_file": 6
    }

    def __init__(self, fname, output_path, format, speaker, speed, parent=None):
        super(MultiDownloader, self).__init__(parent=parent)

        #self.downloader = Download(self)
        #self.downloader.downloadFinished.connect(self.on_one_get)

        self.input = dict()
        self.output = dict()
        self.ERR_FILES = list()

        self.output_path = None
        self.write_file = None

        self._text2urls(fname, output_path, format, speaker, speed)

    def __del__(self):
        self.cancel()

    def run(self):
        #
        print(QThread.currentThread())
        #
        for k, v in self.input.items():
            downloader = Download(k, v, parent=self, ua=choice(user_agents))
            downloader.downloadFinished.connect(self.on_one_get)

    def _prepare_urls(self, BUF, format, speaker, speed):
        text = ""
        index = 0
        for i in range(len(BUF)):
            line = BUF[i]
            if len(QUrl.toPercentEncoding(text)) + len(QUrl.toPercentEncoding(line)) + 1 < MAX_TEXT_URL:
                text += "{}. ".format(line)
            else:
                self.input[index] = url.format(text=text.strip(),
                                           format=format,
                                           speaker=speaker,
                                           speed=speed)
                index += 1
                text = ""

    def _text2urls(self, fname, output_path, format, speaker, speed):
        self._clear()

        self.output_path = output_path

        read_file = QFile(fname)
        if not read_file.open(QIODevice.ReadOnly):
            self.all_done.emit(self.errors['read_file'],
                               self.output_path,
                               -1)
            if read_file.isOpen():
                read_file.close()
            self.cancel()
            return

        self.write_file = QFile(self.output_path)
        if not self.write_file.open(QIODevice.WriteOnly):
            self.all_done.emit(self.errors['write_file'],
                               self.output_path,
                               -1)
            if self.write_file.isOpen():
                self.write_file.close()
            self.cancel()
            return

        buf = read_file.read(MAX_FILE_SIZE)
        mtype = QMimeDatabase().mimeTypeForData(buf).name()
        read_file.close()

        if mtype != 'text/plain':
            self.all_done.emit(self.errors['unsupported_input_file'],
                               self.output_path,
                               -1)
            self.cancel()
            return

        charset = chardet.detect(buf[:1024])
        c = charset['encoding']
        try:
            s = buf.decode(c)
        except:
            self.all_done.emit(self.errors['unsupported_input_file'],
                               self.output_path,
                               -1)
            self.cancel()
            del buf
            return

        del buf

        s = re.sub(r'[\t\r\f\v]', "", s)
        s = re.sub(r'\.\n|\n', ". ", s)
        s = s.replace("   ", " ").replace("  ", " ").replace("..", ".")

        slines = s.split(". ")

        # наполним self.input
        self._prepare_urls(slines, format, speaker, speed)
        del slines

        self.max_buf_len = len(self.input.keys())

        self.output_path = output_path
        #self.downloader.cancelled = False

    def on_one_get(self, ret, num, array):
        if ret == 0:
            self.output[num] = array.data()
        else:
            self.ERR_FILES.append(num)

        self.progressChanged.emit(len(self.ERR_FILES) + len(self.output.keys()), self.max_buf_len)

        if len(self.output.keys()) >= len(self.input.keys()):
            if self.write_file.isWritable():
                # пишем в выходной файл в правильной последовательсноти
                for k in sorted(self.output.keys()):
                    self.write_file.write(self.output[k])
                self.write_file.close()
                if self.ERR_FILES:
                    print('has err')
                    self.all_done.emit(self.errors['write_with_errors'],
                                       self.output_path,
                                       len(self.ERR_FILES))
                else:
                    self.all_done.emit(self.errors['ok'],
                                       self.output_path,
                                       0)
            else:
                self.all_done.emit(self.errors['write_file'],
                                   self.output_path,
                                   -1)
                self.cancel()

    def _clear(self):
        self.output.clear()
        self.input.clear()
        self.ERR_FILES.clear()
        self.output_path = None
        self.write_file = None

    def cancel(self):
        self._clear()


class Voicer(QObject):
    cancel = pyqtSignal()

    def __init__(self, parent=None, max_threads=QThread.idealThreadCount()):
        super(Voicer, self).__init__(parent=parent)
        self.max_threads = max_threads

    def do(self, input, out, fmt, speaker, speed):
        print("Scoring begins...")

        task = MultiDownloader(input, out, fmt, speaker, speed, parent=self)

        #glob_threads = QThreadPool.globalInstance()
        #glob_threads.setParent(self)
        #glob_threads.setMaxThreadCount(self.max_threads)

        #glob_threads.start(task)
        #
        #print(glob_threads.activeThreadCount())
        #
        task.run()

        task.progressChanged.connect(self.on_progress)
        task.all_done.connect(self.on_all_done)

    def on_progress(self, prgs, all):
        print("Progress: {:.2%}".format(prgs/all), end="\r")

    def on_all_done(self, ret_code, ofile, err_nums):
        print("\nFinished. ", end="")
        if err_nums > 0:
            print()
            print("We have errors! Some peases is not voiced",
                  file=sys.stderr)
        else:
            print("Success!")
            print("Output file is {}".format(ofile))
        app_exit(ret_code)

def main():
    parser = argparse.ArgumentParser(description="Translate text to voice")
    parser.add_argument('-o', '--out', help="Outpu File name", required=True)
    parser.add_argument('-i', '--input', help="Input *.txt file", required=True)
    parser.add_argument("-s", '--speaker', default="zahar", help="Speeker",
                        choices=["jane", "oksana", "alyss", "omazh", "zahar", "ermil"])
    parser.add_argument('--speed', default=0.9, help="Output audio speed. 0.1 .. 3.0", type=float)

    args = parser.parse_args()

    if len(args.input.split(".")) >= 2:
        if args.input.split(".")[-1] != "txt":
            print(args.input)
            print("Need .txt file", file=sys.stderr)
            sys.exit(1)
    else:
        print(args.input)
        print("Need .txt file", file=sys.stderr)
        sys.exit(1)

    if 0.1 < args.speed > 3.0:
        print("Speed must be less then 3.0 and more then 0.1", file=sys.stderr)
        parser.print_help()
        sys.exit(1)

    voicer = Voicer(app)

    voicer.do(args.input, args.out, "mp3", args.speaker, args.speed)

    sys.exit(app.exec_())


def app_exit(code=0):
    QCoreApplication.instance().exit(code)
    #sys.exit(code)

signal.signal(signal.SIGINT, signal.SIG_DFL)
app = QCoreApplication(sys.argv)

if __name__ == "__main__":
    main()