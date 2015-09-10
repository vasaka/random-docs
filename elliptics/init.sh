#!/bin/bash

mkdir -p /etc/elliptics
mkdir -p /var/tmp/cocaine/ipc
mkdir -p /var/tmp/cocaine/spool
mkdir -p /elliptics/history
mkdir -p /elliptics/eblob

curl https://raw.githubusercontent.com/vasaka/random-docs/master/elliptics/ioserv_srw.json -o ioserv_srw.json
curl https://raw.githubusercontent.com/vasaka/random-docs/master/elliptics/srw.json -o srw.json

cp ioserv_srw.json /etc/elliptics
cp srw.json /etc/elliptics

#dd bs=64 count=10 if=/dev/urandom of=/elliptics/history/ids

dnet_ioserv -c /etc/elliptics/ioserv_srw.json
