curl http://ftp.cz.debian.org/debian/pool/main/p/python-opster/python-opster_4.1-1_all.deb -o python-opster_4.1-1_all.deb
curl http://ftp.cz.debian.org/debian/pool/main/p/python-tornado/python-tornado_3.2.2-1.1_amd64.deb -o python-tornado_3.2.2-1.1_amd64.deb
dpkg -i python-opster_4.1-1_all.deb python-tornado_3.2.2-1.1_amd64.deb
apt-get -f install -y
