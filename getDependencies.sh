#!/bin/bash

DEPS_DIR=_deps

GOOGLE_TEST_VERSION="1.12.1"
GOOGLE_TEST_ZIP=release-${GOOGLE_TEST_VERSION}.zip
GOOGLE_TEST_DIR=${DEPS_DIR}/googletest

https://github.com/zeux/pugixml/releases/download/v1.15/pugixml-1.15.tar.gz

PUGIXML_VERSION="1.15"
PUGIXML_ARCHIVE=pugixml-${PUGIXML_VERSION}
PUGIXML_ZIP=${PUGIXML_ARCHIVE}.tar.gz
PUGIXML_DOWNLOAD_LINK=https://github.com/zeux/pugixml/releases/download/v${PUGIXML_VERSION}/${PUGIXML_ZIP}
PUGIXML_DIR=${DEPS_DIR}/pugixml

function getGoogleTests(){
    echo "downloading googletest"
    if [ ! -d "$GOOGLE_TEST_DIR" ]; then
        wget https://github.com/google/googletest/archive/$GOOGLE_TEST_ZIP
        unzip $GOOGLE_TEST_ZIP
        mv googletest-release-$GOOGLE_TEST_VERSION $GOOGLE_TEST_DIR
        rm $GOOGLE_TEST_ZIP
    else
        echo "$GOOGLE_TEST_DIR already present!"
    fi
}

function getPugiXml(){
    echo "downloading pugixml"
    if [ ! -d "$PUGIXML_DIR" ]; then
        wget ${PUGIXML_DOWNLOAD_LINK}
        tar -zxf ${PUGIXML_ZIP}
        mv ${PUGIXML_ARCHIVE} ${PUGIXML_DIR}
        rm ${PUGIXML_ZIP}
    else
        echo "$PUGIXML_DIR already present!"
    fi
}

if [ ! -d $DEPS_DIR ]; then
    mkdir $DEPS_DIR
fi

getGoogleTests
getPugiXml
