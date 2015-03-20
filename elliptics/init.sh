#!/bin/bash

mkdir -p /etc/elliptics
mkdir -p /var/tmp/cocaine/ipc
mkdir -p /var/tmp/cocaine/spool
mkdir -p /elliptics/history
mkdir -p /elliptics/eblob

cp ioserv_srw.json /etc/elliptics
cp srw.json /etc/elliptics

#dd bs=64 count=10 if=/dev/urandom of=/elliptics/history/ids

dnet_ioserv -c /etc/elliptics/ioserv_srw.json
