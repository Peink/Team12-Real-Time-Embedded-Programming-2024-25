#!/bin/sh
FILE_PATH=$1
LZ4_FILE_NAME=$2

if [ -e "${FILE_PATH}" ]; then
    lz4 -f ${FILE_PATH} /tmp/${LZ4_FILE_NAME}.lz4 2>/dev/null >/dev/null
    if [ -e "/tmp/${LZ4_FILE_NAME}.lz4" ]; then
        echo "compress success"
    else
        echo "compress fail"
    fi
else
    echo "file is not exist"
fi
