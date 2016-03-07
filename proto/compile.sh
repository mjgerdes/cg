#! /bin/sh

protoc -I./ --cpp_out=../src/msg ./*.proto
protoc -I./ --js_out=../html/msg ./*.proto
