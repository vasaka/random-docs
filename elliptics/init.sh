#!/bin/bash

mkdir -p /etc/elliptics
mkdir -p /var/tmp/cocaine/ipc
mkdir -p /var/tmp/cocaine/spool
mkdir -p /elliptics/history
mkdir -p /elliptics/eblob

cp ioserv_srw.json /etc/elliptics
cp srw.json /etc/elliptics

dnet_ioserv -c cp ioserv_srw.json /etc/elliptics/ioserv_srw.json
