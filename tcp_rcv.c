/* tcp_rcv.c 
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


/* Options and their defaults */
char *o_ip = NULL;
int o_keepalive = 0;
char *o_outfile = NULL;
int o_port = 0;


FILE *outfile_fp = NULL;


char usage_str[] = "Usage: tcp_rcv [-h] [-i ip] [-k keepalive] [-o outfile] [-p port]";

void usage(char *msg) {
  if (msg) fprintf(stderr, "%s\n", msg);
  fprintf(stderr, "%s\n", usage_str);
  exit(1);
}

void help() {
  fprintf(stderr, "%s\n", usage_str);
  fprintf(stderr, "Where:\n"
      "  -h : Print help.\n"
      "  -i ip : IP address to connect to (sets client mode).\n"
      "  -k keepalive : Seconds between TCP keepalive probes (0=none).\n"
      "  -o outfile : Program output will be written to screen and this file.\n"
      "  -p port : Listener port.\n");
  exit(0);
}

void get_opts(int argc, char **argv)
{
  int opt;

  while ((opt = cprt_getopt(argc, argv, "hi:k:o:p:")) != EOF) {
    switch (opt) {
      case 'h':
        help();
        break;
      case 'i':
        if (o_ip != NULL) { free(o_ip); }
        CPRT_ENULL(o_ip = CPRT_STRDUP(cprt_optarg));
        break;
      case 'k':
        CPRT_ATOI(cprt_optarg, o_keepalive);
        break;
      case 'o':
        if (o_outfile != NULL) { free(o_outfile); }
        CPRT_ENULL(o_outfile = CPRT_STRDUP(cprt_optarg));
        break;
      case 'p':
        CPRT_ATOI(cprt_optarg, o_port);
        break;
      case '?':
        fprintf(stderr, "cprt_optopt='%c', Use '-h' for help\n", cprt_optopt);
        exit(1);
        break;
      default:
        fprintf(stderr, "Undefined option '%c' (use -h for help)\n", opt); exit(1);
    }  /* switch opt */
  }  /* while cprt_getopt */

  if (o_port == 0) { fprintf(stderr, "Must supply port (use -h for help)\n"); exit(1); }
  if (o_outfile != NULL) {
    CPRT_ENULL(outfile_fp = fopen(o_outfile, "w"));
  }
}  /* get_opts */


void tcp_rcv()
{
  CPRT_SOCKET listen_sock;
  CPRT_SOCKET sock;
  long actual;
  int retries;
  long total_rcv;
  char rcv_buffer[1024];

  CPRT_EM1(sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP));

  if (o_ip != NULL) { /* Client, connect to IP. */
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    CPRT_EOK1(inet_pton(AF_INET, o_ip, &saddr.sin_addr.s_addr));
    saddr.sin_port = htons(o_port);
    CPRT_EM1(connect(sock, (struct sockaddr *)&saddr, sizeof(saddr)));
  }
  else {  /* Server, listen for connection. */
    struct sockaddr_in saddr;
    socklen_t saddr_len = sizeof(saddr);
    listen_sock = sock;

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(o_port);
    CPRT_EOK0(bind(listen_sock, (struct sockaddr *)&saddr, sizeof(saddr)));
    CPRT_EOK0(listen(listen_sock, 5));

    retries = 5;
    sock = -1;
    while (sock == -1) {
      sock = accept(listen_sock, (struct sockaddr *)&saddr, &saddr_len);
      if (sock == -1) {
        CPRT_PERRNO("accept returned -1: ");
      }
      retries--;
      CPRT_EM1(retries);
    }  /* while */
  }  /* Server. */

  /* If wanted, turn on TCP keepalive. */
  if (o_keepalive > 0) {
    tcp_keepalive(sock, o_keepalive);
  }

  printf("Receive loop\n");  if (outfile_fp) { fprintf(outfile_fp, "Receive loop\n");  fflush(outfile_fp); }
  total_rcv = 0;
  do {
    CPRT_EM1(actual = recv(sock, rcv_buffer, 1, 0));
    if (actual == 0) {
      printf("\nActual=0\n");  if (outfile_fp) { fprintf(outfile_fp, "\nActual=0\n");  fflush(outfile_fp); }
    } else {
      if (rcv_buffer[0] == '\n') {
        printf(".");  fflush(stdout);  if (outfile_fp) { fprintf(outfile_fp, ".");  fflush(outfile_fp); }
      }
    }
    total_rcv += actual;
  } while (actual > 0);
  printf("Received %ld bytes\n", total_rcv);  if (outfile_fp) { fprintf(outfile_fp, "Received %ld bytes\n", total_rcv);  fflush(outfile_fp); }

  CPRT_SOCKET_CLOSE(sock);

  if (o_ip == NULL) { /* Server, have listening socket. */
    CPRT_SOCKET_CLOSE(listen_sock);
  }
}  /* tcp_rcv */


int main(int argc, char **argv)
{
  CPRT_NET_START;

  get_opts(argc, argv);

  tcp_rcv();

  CPRT_NET_CLEANUP;
  return 0;
}  /* main */
