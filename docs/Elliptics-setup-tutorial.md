## Introduction

This tutorial is intended to provide updated info on how to set up elliptics.  
Tutorial assumes that you work on Ubuntu 14.04 or Debian 7, if you want use different distribution the only difference is package setup.

## Overview

Elliptics is fault tolerant distributed key-value storage, for detailed overview please visit http://reverbrain.com/elliptics/

## Preparing system (Ubuntu 14.04)

Add reverbrain repositories to your sources list `/etc/apt/sources.list` by appending following lines
```apt
deb http://repo.reverbrain.com/trusty/ current/amd64/
deb http://repo.reverbrain.com/trusty/ current/all/
```
Add reverbrain key to recognize package signatures.
```bash
curl http://repo.reverbrain.com/REVERBRAIN.GPG | apt-key add -
```
run
```bash
apt-get update
```
and install elliptics
```bash
apt-get install elliptics
```
Or just run the [script](https://github.com/vasaka/random-docs/blob/master/elliptics/ubuntu_install.sh) that does all this.

## Preparing system (Debian 7)

Add reverbrain repositories to your sources list `/etc/apt/sources.list` by appending following lines
```apt
deb http://repo.reverbrain.com/wheezy/ current/amd64/
deb http://repo.reverbrain.com/wheezy/ current/all/
```
Add reverbrain key to recognize package signatures.
```bash
curl http://repo.reverbrain.com/REVERBRAIN.GPG | apt-key add -
```
run
```bash
apt-get update
```
and install elliptics
```bash
apt-get install elliptics
```
Or just run the [script](https://github.com/vasaka/random-docs/blob/master/elliptics/debian_install.sh) that does all this.

## Configuring elliptics

Elliptics is configured with json formatted configuration file, see [example](https://github.com/vasaka/random-docs/blob/master/elliptics/ioserv.json).  
See reverbrain [documentation](http://doc.reverbrain.com/elliptics:configuration) on elliptics configuration parameters, it is good and up to date. [TODO: reverbrain doc mixw numeric and symbolic values for some parameters, find out which are correct].

Lets overview parameters most relative to initial setup.
```json
"remote" : ["localhost:1025:2"],
"address": ["localhost:1025:2-0"]
```
Remotes is a list of nodes to connect format is address:port:family, family is 2 for ipv4 and 10 for ipv6, address is a list of addresses node listens to. address is an address list node listens to format is local_address:port:family-route_group. Documentation claims that you can use autodiscovery list and hostname keyword in address list to automate actual address discovery, but I never managed to make it work [TODO: find out what happened to those features].  
To allow your nodes to connect point each node to several other nodes and add valid listen address in addition or instead of localhost. Nodes will share route information and eventually each node will have full routing table.

```json
"daemon": false
```
With "daemon" set to false and logging to /dev/stderr you can immediately spot if something gone wrong, probably you want to switch it to true and setup proper logging when setup is done.

Also note that all directories mentioned in the config must exist or elliptics will fail to start, for the sample config it is two directories
```bash
mkdir -p /elliptics/history
mkdir -p /elliptics/eblob
```

[TODO: autodiscovery and hostname trick in configuration does not work, find out how to achieve this]

You can run elliptics server node with
```bash
dnet_ioserv -c ioserv.json
```

[TODO: add /etc/init.d script to make proper service]

## Balancing
On first run each node randomly selects a hash range for each 100Gb of free space on the filesystem, if you do not like this default behavior you can create ranges by hand on each node for example
```bash
dd bs=64 count=10 if=/dev/urandom of=/elliptics/history/ids
```
will make 10 random ranges on the node. After that you can run dnet_balancer to achieve better range distribution.
```bash
dnet_balancer -r localhost:1025:2 -g 1 -c
```
to check current distribution
```bash
dnet_balancer -r localhost:1025:2 -g 1 -u
```
to update ranges on nodes.

If you run balancing on the storage that already have the data you should run merge recovery.
More on balancing [here](http://doc.reverbrain.com/elliptics:tools#dnet_balancer)

== Recovery ==

Recovery is described [here](http://doc.reverbrain.com/elliptics:replication)

Questions:  
if we recover keys from dump file how we handle namespaces?
