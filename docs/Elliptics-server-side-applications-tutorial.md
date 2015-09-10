## Introduction

This tutorial is intended to cover the gap in the reverbrain elliptics documentation in server side data processing topic which is outdated and incomplete.

Tutorial assumes that you work on Ubuntu 14.04 or Debian 7, if you want use different distribution the only difference is package setup.  
To proceed with this tutorial you should have elliptics installed, and get some idea how to configure and operate it. you can follow this [tutorial](https://github.com/vasaka/random-docs/wiki/Elliptics-setup-tutorial) for that task  
You also will need following packages installed:
* elliptics
* elliptics-client
* cocaine-tools
* libcocaine-plugin-elliptics
* cocaine-framework-python
* cocaine-framework-native

## Overview

Elliptics has a mechanism to run apps on it's nodes intended for processing data put into storage, app engine is event driven and provide mechanism to run data processing apps locally to the data. App engine is a layer on top of Cocaine, and provides own protocol for managing apps and messages. Attempts to mix Cocaine and elliptics app management lead to Elliptics crashes. [TODO: check that next is true] Each app[TODO: or even app instance?] uses its own Cocaine instance, so you won't get any Cocaine balancing capabilities.

## Debian 7 additional steps

Debian 7 needs some additional work to setup because cocaine-tools depend on some python packages from debian 8 (currently testing)
* python-opster
* python-tornado

```bash
curl http://ftp.cz.debian.org/debian/pool/main/p/python-opster/python-opster_4.1-1_all.deb -o python-opster_4.1-1_all.deb
curl http://ftp.cz.debian.org/debian/pool/main/p/python-tornado/python-tornado_3.2.2-1.1_amd64.deb -o python-tornado_3.2.2-1.1_amd64.deb
dpkg -i python-opster_4.1-1_all.deb python-tornado_3.2.2-1.1_amd64.deb
apt-get -f install -y
```
This will fetch and install proper packages. Command `apt-get -f install -y` fixes dependencies that dpkg left unresolved.

```bash
apt-get install python-pip
pip install backports.ssl_match_hostname
```
this will install dependency for python-tornado that is not in package dependencies. You can use [script](https://github.com/vasaka/random-docs/blob/master/elliptics/debian_install_srw.sh) to automate this task.

_It is better to build package for debian using ubuntu debianization instead of installing package with pip http://packages.ubuntu.com/source/utopic/backports.ssl-match-hostname_

After that you can install packages noramlly.

## Configuring elliptics

Already configured elliptics should have one additional parameter in it's configuration file  
`"srw_config": "/etc/elliptics/srw.json"`  
this is a link to the Cocaine config. [Click for full sample config file](https://github.com/vasaka/random-docs/blob/master/elliptics/ioserv_srw.json).
Note that all directories mentioned in the config must be created before launching Elliptics.  
To run elliptics with this config files put them to /etc/elliptics and run command  
`dnet_ioserv -c /etc/elliptics/ioserv_srw.json`[TODO: I totally omit if it should run as root or own user, have to investigate]

## Configuring cocaine

[Full sample config is under the link](https://github.com/vasaka/random-docs/blob/master/elliptics/srw.json), lets go though it in some details.

Services section defines what services will be used in Cocaine layer  
```json
"services": {
    "logging": {
        "type": "logging",
        "args": {
            "port": 12501
        }
    },
    "storage": {
        "type": "elliptics"
    }
}
```  
logging service and storage service are mandatory, storage should be set to elliptics, uploaded apps would be stored there[TODO: how change to file storage affect app distribution to nodes in case of elliptics managed Cocaine? in standalone Cocaine app will be uploaded to current node only].

[TODO: residual from experiments, probably should be thrown away] 
```json
"node": {
    "type": "node",
    "args": {
        "runlist": "default"
    }
}
```

Storages section sets parameters for elliptics storage, list of nodes to connect, set of groups and logging verbosity
```json
"storages": {
    "core": {
        "type": "elliptics",
        "args": {
            "nodes": ["localhost:1025:2"],
            "groups": [1],
            "verbosity": 2[TODO: is it still a number or word should be used?]
        }
    }
}
```

I prepared [a mighty script](https://github.com/vasaka/random-docs/blob/master/elliptics/init.sh) that gets all initialization done and runs elliptics service as a reference  
It creates all necessary directories and copies config files to the right place
```bash
mkdir -p /etc/elliptics
mkdir -p /var/tmp/cocaine/ipc
mkdir -p /var/tmp/cocaine/spool
mkdir -p /elliptics/history
mkdir -p /elliptics/eblob
cp ioserv_srw.json /etc/elliptics
cp srw.json /etc/elliptics
```
Then runs elliptics with that config
```bash
dnet_ioserv -c /etc/elliptics/ioserv_srw.json
```

Note the commented line
```bash
dd bs=64 count=10 if=/dev/urandom of=/elliptics/history/ids
```
It creates 10 random key ranges, you can use it if you don't like how elliptics create ranges by default.

## Python apps

To run python app in Cocaine you need an app, application manifest, and a profile.  
You will also need `cocaine-framework-python` package installed for python apps.

The minimal python application  
```python
#!/usr/bin/env python
from cocaine.worker import Worker

def test_event(request, response):
    response.close()

W = Worker()
W.run({"test_event" : test_event})
```
This app creates a worker instance and registers a hander for "test_event" which just does nothing.

Manifest describes how application should run, basic one is just a binary name, see [example manifest](https://github.com/vasaka/random-docs/blob/master/elliptics/test_app_manifest.json)
```json
{
    "slave" : "test_app.py",
}
```

Profile describes application isolation, it can be just a [process spawn profile](https://github.com/vasaka/random-docs/blob/master/elliptics/process_isolate.profile) or cgroup or docker
```json

{
    "isolate": {
        "type": "process",
        "args": {
            "spool": "/var/tmp/cocaine/spool"
        }
    }
}
```

To upload this app to the elliptics you should issue [commands](https://github.com/vasaka/random-docs/blob/master/elliptics/install_test_app.sh):
```bash
tar -cf test_app.tar test_app.py
```
Create a tar package with app
```bash
cocaine-tool app upload --manifest test_app_manifest.json --package test_app.tar --name test_app
```
upload app with attached manifest to elliptics using cocaine-tool
```bash
cocaine-tool profile upload --profile process_isolate.profile --name test_app
```
Upload profile[TODO: how profiles are matched to apps?] with name set to app name
```bash
dnet_ioclient -r localhost:1025:2 -g 1 -c "test_app@start-task"
```
Send event to start that app.

You can test that app is working by calling
```bash
dnet_ioclient -r localhost:1025:2 -g 1 -c "test_app@test_event"
```
it should show acknowledge answer from each node, if you want to run app on the node with specific key call
```bash
dnet_ioclient -r localhost:1025:2 -g 1 -c "test_app@test_event key_string"
```
it should show acknowledge from the node with the key.  
[TODO: add sending events from python and cpp code.]

You also can run app using cocaine mechanism instead of elliptics events, in that case you should change the way events are registered[TODO: this thing with handlers bugs me a lot]
```python
W = Worker()
W.run({"test_app@test_event" : test_event})
```
Sample code to run app:
```python
from cocaine.services import Service
app = Service("test_app")
for i in app.enqueue("test_app@test_event", "Hello\n"):
    print i
```

More elaborate [example](https://github.com/vasaka/random-docs/blob/master/elliptics/test_app.py) also writes data we send it to TESTKEY
```python
from cocaine.worker import Worker
import elliptics
import struct

elog = elliptics.Logger("/tmp/worker", 0)
cfg = elliptics.Config()
node = elliptics.Node(elog, cfg)
node.add_remotes(["localhost:1025:2"])

def test_event(request, response):
    try:
         data = yield request.read()
         s = elliptics.Session(node)
         s.set_groups([1])
         event_size = struct.unpack('<i', data[80:84])[0]
         s.write_data("TESTKEY", data[128 + event_size:], 0)
    except Exception as err:
         response.error(1, str(err))
    finally:
         response.close()

W = Worker()
W.run({"test_event" : test_event})
```
Yo can test it with dnet_ioclient
```bash
dnet_ioclient -r localhost:1025:2 -g 1 -c "test_app@test_event SOME_DATA"
```
After command success reading from TESTKEY should return SOME_DATA
```bash
dnet_ioclient -r localhost:1025:2 -g 1 -D TESTKEY
```

Note that script file should have executable flag.

More on [python api](http://doc.reverbrain.com/elliptics:api-python)

## C++ apps

To run C++ app in Cocaine you need a compiled app, application manifest, and a profile.  
You will also need `cocaine-framework-native` package installed for C++ apps.

Minimal C++ application:
```c++
#include <cocaine/framework/dispatch.hpp>
#include <cocaine/framework/logging.hpp>
#include <iostream>

using namespace ioremap;
static void noop_function(cocaine::framework::response_ptr)
{
class app_t {
public:
    std::string id;
    std::shared_ptr<cocaine::framework::logger_t> cocaine_log;
    app_t(cocaine::framework::dispatch_t& dispatch)
         : id(dispatch.id())
         , cocaine_log(dispatch.service_manager()->get_system_logger())
     {
         COCAINE_LOG_INFO(cocaine_log, "%s, registering event handler(s)", id.c_str());
         dispatch.on("test_event", this, &app_t::test_event);
         COCAINE_LOG_INFO(cocaine_log, "%s, application started", id.c_str());
     }
     void test_event(const std::string &cocaine_event, const std::vector<std::string> &chunks, cocaine::framework::response_ptr response) {
         COCAINE_LOG_INFO(cocaine_log, "echo: event: %s, data-size: %ld", cocaine_event.c_str(), context.data().size());
         result.connect(std::bind(noop_function, response));
     }
};
}

int main(int argc, char **argv) {
     return cocaine::framework::run<app_t>(argc, argv);
}
```
This app creates a worker instance and registers a hander for "test_event".  

See the CMake [project](https://github.com/vasaka/random-docs/tree/master/elliptics/test_app_cpp) for details on compilation. In short you should compile with std=c++11 and link against boost_thread, boost_system, boost_iostreams, elliptics_cpp, elliptics_client, cocaine-framework, ev.

[Manifest](https://github.com/vasaka/random-docs/blob/master/elliptics/test_app_manifest_cpp.json), [profile](https://github.com/vasaka/random-docs/blob/master/elliptics/process_isolate.profile) and [upload commands](https://github.com/vasaka/random-docs/blob/master/elliptics/install_test_app_cpp.sh) are pretty much the same as those for python.

Note that compiled binary file should have executable flag.

More on [C++ api](http://doc.reverbrain.com/elliptics:api-cc)

## Python client app

Messages can be sent from python client apps and workers
```python
import elliptics

elog = elliptics.Logger("/tmp/worker", 0)
cfg = elliptics.Config()
node = elliptics.Node(elog, cfg)
node.add_remotes(["localhost:1025:2"])
s = elliptics.Session(node)
s.set_groups([1])
s.exec_(event = "test_app@test_event", data = "MSG\n")
```
Here we create a client node, with logger and default config, adding remotes to bootstrap route table[TODO: should autodiscovery work here?], initialize sesson object with node, set groups we want to talk to and finally sending message with exec_

## C++ client app

As for python messages can be sent from C++ client apps and workers, process is very similar to python one:
```c++
dnet_log_level log_level = ioremap::elliptics::file_logger::parse_level("info");
ioremap::elliptics::file_logger logger("/tmp/send_msg_log", log_level);
ioremap::elliptics::node node(ioremap::elliptics::logger(logger, blackhole::log::attributes_t()));
node.add_remote("localhost:1025:2");

ioremap::elliptics::session session(node);
session.set_groups({1});

session.exec(NULL, -1, "test_app@test_event", "some random data!\n");
```

Here we create a client node object, adding remotes to bootstrap route table[TODO: should autodiscovery work here?], use it to init session, set target group to 1, and send message to all nodes in group with attached data.

More elaborate example, that handles reply and sends message only to one node with target key [here](https://github.com/vasaka/random-docs/tree/master/elliptics/send_msg_cpp).

## App management

Elliptics provides some interface to manage uploaded apps, it is done with special messages
```bash
dnet_ioclient -r localhost:1025:2 -g 1 -c "test_app@start_task"
```
will start the app
```bash
dnet_ioclient -r localhost:1025:2 -g 1 -c "test_app@stop_task"
```
will stop the app
```bash
dnet_ioclient -r localhost:1025:2 -g 1 -c "test_app@info"
```
will output various info about the app, on my setup answer looks like this:
```json
test_app@info "{
   "counters" : {},
   "load-median" : 0,
   "profile" : "test_app",
   "queue" : {
      "capacity" : 100,
      "depth" : 0
   },
   "sessions" : {
      "pending" : 0
   },
   "slaves" : {
      "active" : 0,
      "capacity" : 10,
      "idle" : 0
   },
   "state" : "running"
}
"
```

To update app to the new version, you should upload the new app version, then issue stop, then issue start commands.

Do not mix elliptics and cocaine application management, it crashes elliptics for me all the time.[TODO: should I file some bugs on that?]

questions:
how apps and profiles are matched when elliptics server side processing is used?  
how to write responses in python?  
how to create event handlers that can be called from both cocaine interface and through elliptics messaging?  