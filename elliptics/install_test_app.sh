#!/bin/bash

tar -cf test_app.tar test_app.py
cocaine-tool app upload --manifest test_app_manifest.json --package test_app.tar --name test_app
cocaine-tool profile upload --profile process_isolate.profile --name test_app
dnet_ioclient -r localhost:1025:2 -g 1 -c "test_app@stop-task"
dnet_ioclient -r localhost:1025:2 -g 1 -c "test_app@start-task"
