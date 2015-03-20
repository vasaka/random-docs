#!/bin/bash

tar -cf test_app_cpp.tar test_app_cpp/build/test_app_cpp
cocaine-tool app upload --manifest test_app_manifest_cpp.json --package test_app_cpp.tar --name test_app_cpp
cocaine-tool profile upload --profile process_isolate.profile --name test_app_cpp
dnet_ioclient -r localhost:1025:2 -g 1 -c "test_app_cpp@start-task"
dnet_ioclient -r localhost:1025:2 -g 1 -c "test_app_cpp@init"
