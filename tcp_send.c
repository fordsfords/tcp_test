/* tcp_send.c 
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
int o_lingertime = 0;
int o_num_msgs = 0;
int o_port = 0;
int o_sleeptime = 0;


char usage_str[] = "Usage: cprt_test [-h] [-i ip] [-k keepalive] [-l lingertime] [-n num_msgs] [-p port] [-s sleeptime]";

void usage(char *msg) {
  if (msg) fprintf(stderr, "%s\n", msg);
  fprintf(stderr, "%s\n", usage_str);
  exit(1);
}

void help() {
  fprintf(stderr, "%s\n", usage_str);
  fprintf(stderr, "where:\n"
      "  -h : print help.\n"
      "  -i ip : IP address to connect to (sets client mode).\n"
      "  -k keepalive : Seconds between TCP keepalive probes (0=none).\n"
      "  -l lingertime : Seconds to sleep (after last send) before shutting down.\n"
      "  -n num_msgs : number of hello world messagse to send.\n"
      "  -p port : listener port.\n"
      "  -s sleeptime : Seconds to sleep after first message.\n");
  exit(0);
}

void get_opts(int argc, char **argv)
{
  int opt;

  while ((opt = cprt_getopt(argc, argv, "hi:k:l:n:p:s:")) != EOF) {
    switch (opt) {
      case 'h':
        help();
        break;
      case 'i':
        if (o_ip != NULL) { free(o_ip); }
        o_ip = CPRT_STRDUP(cprt_optarg);
        CPRT_ENULL(o_ip);
        break;
      case 'k':
        CPRT_ATOI(cprt_optarg, o_keepalive);
        break;
      case 'l':
        CPRT_ATOI(cprt_optarg, o_lingertime);
        break;
      case 'n':
        CPRT_ATOI(cprt_optarg, o_num_msgs);
        break;
      case 'p':
        CPRT_ATOI(cprt_optarg, o_port);
        break;
      case 's':
        CPRT_ATOI(cprt_optarg, o_sleeptime);
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
}  /* get_opts */


void tcp_send()
{
  CPRT_SOCKET listen_sock;
  CPRT_SOCKET sock;
  long actual;
  int retries;
  int m;
  char send_buffer[1024];

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

  printf("Send first\n");
  strncpy(send_buffer, "Hello word!\n", sizeof(send_buffer));
  CPRT_EM1(actual = send(sock, send_buffer, (int)strlen(send_buffer), 0));
  if (actual != strlen(send_buffer)) { fprintf(stderr, "send: strlen(send_buffer)=%ld, actual=%ld\n", (long)strlen(send_buffer), (long)actual); }

  printf("sleep %d seconds\n", o_sleeptime);
  CPRT_SLEEP_SEC(o_sleeptime);

  if (o_num_msgs > 1) {
    printf("Sending more");  fflush(stdout);
  }
  /* Start with m=1 because the "Hello world" message was number 0. */
  for (m = 1; m < o_num_msgs; m++) {  /* Countdown messages. */
    printf(".");  fflush(stdout);

    CPRT_SNPRINTF(send_buffer, sizeof(send_buffer), "Message %d\n", m+1);
    CPRT_EM1(actual = send(sock, send_buffer, (int)strlen(send_buffer), 0));
    if (actual != strlen(send_buffer)) { fprintf(stderr, "send: strlen(send_buffer)=%ld, actual=%ld\n", (long)strlen(send_buffer), (long)actual); }

    if (m != (o_num_msgs-1)) {  /* Don't sleep after the last send. */
      CPRT_SLEEP_SEC(1);
    }
  }

  printf("\nLinger before shutdown %d seconds\n", o_lingertime);
  CPRT_SLEEP_SEC(o_lingertime);

  printf("Shutdown\n");
  CPRT_EM1(shutdown(sock, CPRT_SHUT_WR));
  do {
    CPRT_EM1(actual = recv(sock, send_buffer, sizeof(send_buffer), 0));
    if (actual > 0) {
      fprintf(stderr, "recv: %ld bytes read while waiting for EOF.\n", (long)actual);
    }
  } while (actual > 0);

  printf("Closing\n");
  CPRT_SOCKET_CLOSE(sock);

  if (o_ip == NULL) { /* Server, have listening socket. */
    CPRT_SOCKET_CLOSE(listen_sock);
  }
}  /* tcp_send */


int main(int argc, char **argv)
{
  CPRT_NET_START;

  get_opts(argc, argv);

  tcp_send();

  CPRT_NET_CLEANUP;
  return 0;
}  /* main */
