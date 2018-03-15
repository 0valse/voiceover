#!/usr/bin/env python3

import sys
import requests
import argparse
from random import choice
from os import path


KEY="a3a66d18-e9d2-43af-9408-36a30416baed"
url = 'https://tts.voicetech.yandex.net/generate?text="{}"&format=ogg&lang=ru-RU&speaker=zahar&emotion=good&key=%s' % KEY

headers = {
    'Accept': '*/*',
    'Accept-Encoding': 'gzip, deflate, br',
    'Accept-Language': 'ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7'
}

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

def get(text):
    hdr = dict(headers)
    hdr["User-Agent"] = choice(user_agents)
    r = requests.get(url.format(text), headers=hdr)
    if r.status_code != 200:
        return
    return r.content


def get_fname(tmpl, counter, LEN):
    t = "{}-{:0>%d}" % LEN
    return t.format(tmpl, counter)


def main(wdir, fname, ftmpl):
    ERR_FILES = list()
    COUNTER = 0
    BUF = list()

    with open(fname, "rt") as f:
        while True:
            buf = f.readline()
            if buf:
                if buf.strip():
                    BUF.append(buf.strip())
            else:
                break
    del buf

    LEN = len(str(len(BUF))) # digit counts

    for i in BUF:
        if i:
            COUNTER += 1
            name = path.join(wdir, get_fname(ftmpl, COUNTER, LEN) + ".ogg")

            print (name, len(i))

            ogg = get(i)
            if ogg is not None:
                with open(name, "bw") as wf:
                    wf.write(ogg)
            else:
                print("Err while voiceover {}".format(ogg),
                      file=sys.stderr)
                ERR_FILES.append(ogg)
    if ERR_FILES:
        print("Some files not speeches {}".format(ERR_FILES),
              file=sys.stderr)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Translate text to voice")
    parser.add_argument('-d', '--dir', default="./", help="Working dir")
    parser.add_argument('-n', '--name', default="mybook", help="File names template")
    parser.add_argument('-f', '--file', help="Text file")
    args = parser.parse_args()


    if len(sys.argv) <= 1:
        parser.print_help()
        sys.exit(1)

    main(args.dir, args.file, args.name)
    exit(0)
