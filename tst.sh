#!/bin/sh
# tst.sh - script to test the tcp_test programs.

fail() {
  echo "FAIL at $(caller)"
  exit 1
}


# Disable core files
ulimit -c 0


