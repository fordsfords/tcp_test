#!/bin/sh


OPTS=""
if echo "$OSTYPE" | egrep -i linux >/dev/null; then :
  OPTS="-pthread"
fi

gcc -Wall -o tcp_send $OPTS cprt.c tcp_send.c
if [ $? -ne 0 ]; then exit 1; fi

gcc -Wall -o tcp_rcv $OPTS cprt.c tcp_rcv.c
if [ $? -ne 0 ]; then exit 1; fi
