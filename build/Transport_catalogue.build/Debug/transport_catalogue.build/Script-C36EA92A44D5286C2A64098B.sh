#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/user/learning_temp/c_plus/sprint_nine/transport_catalogue/build
  /Users/user/protobuf/protobuf-3.19.4/bin/protoc --cpp_out /Users/user/learning_temp/c_plus/sprint_nine/transport_catalogue/build -I /Users/user/learning_temp/c_plus/sprint_nine/transport_catalogue /Users/user/learning_temp/c_plus/sprint_nine/transport_catalogue/map_renderer.proto
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/user/learning_temp/c_plus/sprint_nine/transport_catalogue/build
  /Users/user/protobuf/protobuf-3.19.4/bin/protoc --cpp_out /Users/user/learning_temp/c_plus/sprint_nine/transport_catalogue/build -I /Users/user/learning_temp/c_plus/sprint_nine/transport_catalogue /Users/user/learning_temp/c_plus/sprint_nine/transport_catalogue/map_renderer.proto
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/user/learning_temp/c_plus/sprint_nine/transport_catalogue/build
  /Users/user/protobuf/protobuf-3.19.4/bin/protoc --cpp_out /Users/user/learning_temp/c_plus/sprint_nine/transport_catalogue/build -I /Users/user/learning_temp/c_plus/sprint_nine/transport_catalogue /Users/user/learning_temp/c_plus/sprint_nine/transport_catalogue/map_renderer.proto
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/user/learning_temp/c_plus/sprint_nine/transport_catalogue/build
  /Users/user/protobuf/protobuf-3.19.4/bin/protoc --cpp_out /Users/user/learning_temp/c_plus/sprint_nine/transport_catalogue/build -I /Users/user/learning_temp/c_plus/sprint_nine/transport_catalogue /Users/user/learning_temp/c_plus/sprint_nine/transport_catalogue/map_renderer.proto
fi

