#!/usr/bin/env python3

# first generate python bindings like so
# for f in  ../../proto/*.proto; do protoc -I=../../proto --python_out=. $f; done

import api_pb2
import os

# for each pbf
for root, dirs, files in os.walk(".", topdown=False):
  for name in files:
    if name.endswith('.pbf'):
      # read the protobuf from disk
      with open(os.path.join(root, name), 'rb') as handle:
        pbf = handle.read()
      api = api_pb2.Api()
      api.ParseFromString(pbf)

      # MAKE YOUR CHANGES TO THE PROTOBUF HERE vvvvvvv
      api.options.directions_type = api_pb2.DirectionsType.Value('instructions')
      api.options.language = 'en-US'
      api.options.roundabout_exits = True
      # MAKE YOUR CHANGES TO THE PROTOBUF HERE ^^^^^^^

      # write the protobuf back to disk
      with open(os.path.join(root, name), 'wb') as handle:
        handle.write(api.SerializeToString())
      print('Wrote %s' % os.path.join(root, name))
