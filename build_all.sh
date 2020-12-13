#!/bin/bash
cmake .
make
cd qt_app
mkdir -p build
cd build
qmake ..
make
cd ../..
doxygen
cd doc/latex
make
cd ../..

mkdir -p artifuckts/usr/lib64
mkdir -p artifuckts/usr/share/multifm/doc/pdf
mkdir -p artifuckts/usr/share/multifm/doc/html
mkdir -p artifuckts/usr/bin
mkdir -p artifuckts/usr/include/cutedsp

cp src/libcutedsp.so artifuckts/usr/lib64
cp doc/latex/refman.pdf artifuckts/usr/share/multifm/doc/pdf
cp -r doc/html/* artifuckts/usr/share/multifm/doc/html
cp src/multifm_console_app artifuckts/usr/bin
cp qt_app/build/qt_app artifuckts/usr/bin/multifm
cp inc/* artifuckts/usr/include/cutedsp

rm -f multifm.tar.gz
tar -czf multifm.tar.gz -C artifuckts .
