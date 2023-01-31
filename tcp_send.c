/* tcp_send.c 
 * This tries to be portable between Mac, Linux, and Windows.
 * See https://github.com/fordsfords/tcp_test
 */
/*
# This code and its documentation is Copyright 2002-2021 Steven Ford
# and licensed "public domain" style under Creative Commons "CC0":
#   http://creativecommons.org/publicdomain/zero/1.0/
# To the extent possible under law, the contributors to this project have
# waived all copyright and related or neighboring rights to this work.
# In other words, you can use this code for any purpose without any
# restrictions.  This work is published from: United States.  The project home
# is https://github.com/fordsfords/tcp_test
*/

#include "cprt.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <errno.h>


/* Options and their defaults */
char *o_ip = NULL;
int o_port = 0;
int o_testnum = 0;


char usage_str[] = "Usage: cprt_test [-h] [-t testnum] [unused_arg]";

void usage(char *msg) {
  if (msg) fprintf(stderr, "%s\n", msg);
  fprintf(stderr, "%s\n", usage_str);
  exit(1);
}

void help() {
  fprintf(stderr, "%s\n", usage_str);
  fprintf(stderr, "where:\n"
      "  -h : print help\n"
      "  -i ip : IP address to connect to (client)\n"
      "  -t testnum : run specified test\n"
      "  unused_arg : optional argument printed in test 10 (if supplied)\n");
  exit(0);
}

void get_opts(int argc, char **argv)
{
  int opt;

  while ((opt = cprt_getopt(argc, argv, "hci:p:t:")) != EOF) {
    switch (opt) {
      case 'h':
        help();
        break;
      case 'i':
        if (o_ip != NULL) { free(o_ip); }
        o_ip = CPRT_STRDUP(cprt_optarg);
        CPRT_ENULL(o_ip);
        break;
      case 'p':
        CPRT_ATOI(cprt_optarg, o_port);
        break;
      case 't':
        CPRT_ATOI(cprt_optarg, o_testnum);
        break;
      case '?':
        fprintf(stderr, "cprt_optopt='%c', Use '-h' for help\n", cprt_optopt);
        exit(1);
        break;
      default:
        fprintf(stderr, "Undefined option '%c'\n", opt);
        exit(1);
    }  /* switch opt */
  }  /* while cprt_getopt */
}  /* get_opts */


void tcp_send()
{
  CPRT_SOCKET sock;


  CPRT_EM1(sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP));

  if (o_ip != NULL) {
    /* Client, connect to IP. */
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    CPRT_EOK1(inet_pton(AF_INET, o_ip, &saddr.sin_addr.s_addr));
    saddr.sin_port = htons(o_port);
    CPRT_EM1(connect(sock, (struct sockaddr *)&saddr, sizeof(saddr)));
  }

}  /* tcp_send */


int main(int argc, char **argv)
{
  CPRT_NET_START;

  get_opts(argc, argv);

  CPRT_INITTIME();

  switch(o_testnum) {
    case 0:
      fprintf(stderr, "test %d: GETOPT_PORT, CPRT_NET_START\n", o_testnum);
      fflush(stderr);
      tcp_send();
      break;
    default:
      fprintf(stderr, "test %d: no such test\n", o_testnum);
  }  /* switch o_testnum */


  CPRT_NET_CLEANUP;
  return 0;
}  /* main */
