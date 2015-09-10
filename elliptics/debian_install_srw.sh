#!/bin/bash

wget http://ftp.cz.debian.org/debian/pool/main/p/python-opster/python-opster_4.1-1_all.deb
wget http://ftp.cz.debian.org/debian/pool/main/p/python-tornado/python-tornado_3.2.2-1.1_amd64.deb
dpkg -i python-opster_4.1-1_all.deb python-tornado_3.2.2-1.1_amd64.deb
apt-get -f install -y

apt-get install python-pip -y
pip install backports.ssl_match_hostname
