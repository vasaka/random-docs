#!/usr/bin/env python

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
