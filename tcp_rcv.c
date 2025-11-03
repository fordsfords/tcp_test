/* tcp_rcv.c 
 * This tries to be portable between Mac, Linux, and Windows.
 * See https://github.com/fordsfords/tcp_test */

/* This work is dedicated to the public domain under CC0 1.0 Universal:
 * http://creativecommons.org/publicdomain/zero/1.0/
 * 
 * To the extent possible under law, Steven Ford has waived all copyright
 * and related or neighboring rights to this work. In other words, you can 
 * use this code for any purpose without any restrictions.
 * This work is published from: United States.
 * Project home: https://github.com/fordsfords/tcp_test
 */

#include "cprt.h"
#include "tcp_utils.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <errno.h>


/* Options and their defaults */
char *o_ip = NULL;  /* Default is set in get_opts(). */
int o_keepalive = 0;
char *o_outfile = NULL;  /* Default is set in get_opts(). */
int o_port = 0;


FILE *outfile_fp = NULL;
char id_line[2000];


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
  struct cprt_timeval tv;
  struct tm out_tm;
  char run_time[80];

  /* Defaults */
  o_ip = CPRT_STRDUP("");
  o_outfile = CPRT_STRDUP("");

  while ((opt = cprt_getopt(argc, argv, "hi:k:o:p:")) != EOF) {
    switch (opt) {
      case 'h':
        help();
        break;
      case 'i':
        free(o_ip);
        CPRT_ENULL(o_ip = CPRT_STRDUP(cprt_optarg));
        break;
      case 'k':
        CPRT_ATOI(cprt_optarg, o_keepalive);
        break;
      case 'o':
        free(o_outfile);
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
  if (strlen(o_outfile) > 0) {
    CPRT_ENULL(outfile_fp = fopen(o_outfile, "w"));
  }

  /* Get timestamp. */
  CPRT_EOK0(CPRT_TIMEOFDAY(&tv, NULL));  CPRT_LOCALTIME_R(&(tv.tv_sec), &out_tm);
  CPRT_SNPRINTF(run_time, sizeof(run_time), "%04d/%02d/%02d %02d:%02d:%02d.%06d",
      (int)out_tm.tm_year + 1900, (int)out_tm.tm_mon + 1, (int)out_tm.tm_mday,
      (int)out_tm.tm_hour, (int)out_tm.tm_min, (int)out_tm.tm_sec, (int)tv.tv_usec);

  CPRT_SNPRINTF(id_line, sizeof(id_line), "Build: %s %s, run: %s; equiv cmd: %s  -i %s -k %d -o %s -p %d\n",
      __DATE__, __TIME__, run_time, argv[0], strlen(o_ip)?o_ip:"\"\"", o_keepalive, strlen(o_outfile)?o_outfile:"\"\"", o_port);
  printf("%s", id_line);  if (outfile_fp) { fprintf(outfile_fp, "%s", id_line);  fflush(outfile_fp); }
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

  if (strlen(o_ip) > 0) { /* Client, connect to IP. */
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
    printf("connection accepted\n"); if (outfile_fp) { fprintf(outfile_fp, "connection accepted\n");  fflush(outfile_fp); }
  }  /* Server. */

  /* If wanted, turn on TCP keepalive. */
  if (o_keepalive > 0) {
    tcp_keepalive(sock, o_keepalive);
  }

  printf("Receive loop:\n");  if (outfile_fp) { fprintf(outfile_fp, "Receive loop:\n");  fflush(outfile_fp); }
  total_rcv = 0;
  do {
    CPRT_EM1(actual = recv(sock, rcv_buffer, 1, 0));
    if (actual == 0) {
      printf("Actual=0\n");  if (outfile_fp) { fprintf(outfile_fp, "Actual=0\n");  fflush(outfile_fp); }
    } else {
      printf("%c", rcv_buffer[0]);  fflush(stdout);  if (outfile_fp) { fprintf(outfile_fp, "%c", rcv_buffer[0]);  fflush(outfile_fp); }
    }
    total_rcv += actual;
  } while (actual > 0);
  printf("Received %ld bytes\n", total_rcv);  if (outfile_fp) { fprintf(outfile_fp, "Received %ld bytes\n", total_rcv);  fflush(outfile_fp); }

  CPRT_SOCKET_CLOSE(sock);

  if (strlen(o_ip) == 0) { /* Server, have listening socket. */
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
