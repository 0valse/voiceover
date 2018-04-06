#!/usr/bin/env python3
import glob
from setuptools import setup
from setuptools import find_packages

APP_NAME = "ivoicer"

requirements = [
    "chardet",
    "PyQt5"
]

setup(
    name=APP_NAME,
    version='2.0a0',
    packages=find_packages(),
    url='https://github.com/0valse/voiceover/',
    license='GPL-3',
    author='Sasha Nenudny',
    author_email='sasha.nenudny@gmail.com',
    description='Alpha version',
    long_description="Voice-over of the text in Russian with Yandex SpeechKit",
    classifiers = (
        'Development Status :: 3 - Alfa',
        'Environment :: Console',
        'Intended Audience :: End Users/Desktop',
        'Operating System :: POSIX :: Linux',
        'Natural Language :: English',
        'Natural Language :: Russian',
        ),
    scripts=['scripts/ivoicer'],
    install_requires=requirements,
)
