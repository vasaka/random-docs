#!/bin/bash

apt-get update
apt-get install curl -y

echo >> /etc/apt/sources.list
echo "deb http://repo.reverbrain.com/trusty/ current/amd64/" >> /etc/apt/sources.list
echo "deb http://repo.reverbrain.com/trusty/ current/all/" >> /etc/apt/sources.list

curl http://repo.reverbrain.com/REVERBRAIN.GPG | apt-key add -

apt-get update
apt-get install elliptics -y
