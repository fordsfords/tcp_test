/* tcp_utils.c 
 * This tries to be portable between Mac, Linux, and Windows.
 * See https://github.com/fordsfords/tcp_test
 */
/*
# This code and its documentation is Copyright 2022-2023 Steven Ford
# and licensed "public domain" style under Creative Commons "CC0":
#   http://creativecommons.org/publicdomain/zero/1.0/
# To the extent possible under law, the contributors to this project have
# waived all copyright and related or neighboring rights to this work.
# In other words, you can use this code for any purpose without any
# restrictions.  This work is published from: United States.  The project home
# is https://github.com/fordsfords/tcp_test
*/

#include "cprt.h"
#include "tcp_utils.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#if defined(_WIN32)
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <netinet/tcp.h>
#endif


void tcp_keepalive(CPRT_SOCKET sock, int interval)
{
#if defined(_WIN32)
  DWORD ret;
  struct tcp_keepalive tcp_ka, tcp_ka_ret;

  tcp_ka.onoff = 1;
  tcp_ka.keepalivetime = (long)interval * 1000;
  tcp_ka.keepaliveinterval = (long)interval * 1000;

  CPRT_EOK0(WSAIoctl(sock, SIO_KEEPALIVE_VALS, &tcp_ka, sizeof(tcp_ka), &tcp_ka_ret, sizeof(tcp_ka), &ret, NULL, NULL));
#else
  int ka = 1;
  int keepcnt = 9;
  int keepidle = interval;
  int keepintvl = interval;

  CPRT_EOK0(setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (const char *)&ka, sizeof(ka)));
  CPRT_EOK0(setsockopt(sock, SOL_TCP, TCP_KEEPCNT, (const char *)&keepcnt, sizeof(keepcnt)));
  CPRT_EOK0(setsockopt(sock, SOL_TCP, TCP_KEEPIDLE, (const char *)&keepidle, sizeof(keepidle)));
  CPRT_EOK0(setsockopt(sock, SOL_TCP, TCP_KEEPINTVL, (const char *)&keepintvl, sizeof(keepintvl)));
#endif
}  /* tcp_keepalive */
