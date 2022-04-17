#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/user/learning_temp/c_plus/sprint_nine/transport_catalogue/build
  make -f /Users/user/learning_temp/c_plus/sprint_nine/transport_catalogue/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/user/learning_temp/c_plus/sprint_nine/transport_catalogue/build
  make -f /Users/user/learning_temp/c_plus/sprint_nine/transport_catalogue/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/user/learning_temp/c_plus/sprint_nine/transport_catalogue/build
  make -f /Users/user/learning_temp/c_plus/sprint_nine/transport_catalogue/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/user/learning_temp/c_plus/sprint_nine/transport_catalogue/build
  make -f /Users/user/learning_temp/c_plus/sprint_nine/transport_catalogue/build/CMakeScripts/ReRunCMake.make
fi

