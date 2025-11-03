/* tcp_utils.h - for portability. */

/* This work is dedicated to the public domain under CC0 1.0 Universal:
 * http://creativecommons.org/publicdomain/zero/1.0/
 * 
 * To the extent possible under law, Steven Ford has waived all copyright
 * and related or neighboring rights to this work. In other words, you can 
 * use this code for any purpose without any restrictions.
 * This work is published from: United States.
 * Project home: https://github.com/fordsfords/tcp_test
 */

#ifndef TCP_UTILS_H
#define TCP_UTILS_H

#include "cprt.h"

#ifdef __cplusplus
extern "C" {
#endif

void tcp_keepalive(CPRT_SOCKET sock, int interval);

#if defined(__cplusplus)
}
#endif

#endif  /* TCP_UTILS_H */
