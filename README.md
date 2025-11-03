# tcp_test
TCP server and client portable across Mac, Linux, and Windows.

# Table of contents

<!-- mdtoc-start -->
&bull; [tcp_test](#tcp_test)  
&bull; [Table of contents](#table-of-contents)  
&bull; [INTRODUCTION](#introduction)  
&bull; [STATEFUL ROUTER TIMING OUT IDLE CONNECTIONS](#stateful-router-timing-out-idle-connections)  
&bull; [HELP](#help)  
&bull; [TIPS](#tips)  
&bull; [LICENSE](#license)  
<!-- TOC created by '../mdtoc/mdtoc.pl README.md' (see https://github.com/fordsfords/mdtoc) -->
<!-- mdtoc-end -->

# INTRODUCTION

This repository is for misc TCP test tools.
At present, it is primarily for testing TCP's keepalive feature.

The approach is simple:
* Two programs, "tcp_send" and "tcp_rcv".
* Either one can function as a server or client.
The client initiates a connection to the server.
* Either or both can enable TCP keepalives.
* The sender waits for the connection to be established.
* The sender does the following:
````
1. Send the first message.
2. Pause for "-s sleeptime" seconds (usually many minutes).
3. Send a specified number of additional messages, one per second.
````

During step 2's sleep,
the connection can be examined using a packet capture tool (like Wireshark)
to verify the presence or absence of keepalives.

# STATEFUL ROUTER TIMING OUT IDLE CONNECTIONS

A good use case for these tools is to determine if an intervening
router between two hosts maintains state for TCP connections,
and if so, if it times out idle connections.

This is common for routers that implement
[NAT (Network Address Translation)](https://en.wikipedia.org/wiki/Network_address_translation).
TCP connections consume router resources and must be cleaned up
to prevent resource exhaustion.


# HELP

````
Usage: tcp_send [-h] [-i ip] [-k keepalive] [-l lingertime] [-n num_msgs] [-p port] [-s sleeptime]
where:
  -h : print help.
  -i ip : IP address to connect to (sets client mode).
  -k keepalive : Seconds between TCP keepalive probes (0=none).
  -l lingertime : Seconds to sleep (after last send) before shutting down.
  -n num_msgs : number of hello world messages to send.
  -p port : listener port.
  -s sleeptime : Seconds to sleep after first message.
````

Note that tcp_send can be run as either a client (by supplying "-i ip" of the server)
or as a server (by omitting "-i ip").
In either case, tcp_send waits until the connection is fully established
before it starts sending.


````
Usage: tcp_rcv [-h] [-i ip] [-k keepalive] [-p port]
where:
  -h : print help.
  -i ip : IP address to connect to (sets client mode).
  -k keepalive : Seconds between TCP keepalive probes (0=none).
  -p port : listener port.
````

Note that tcp_rcv can be run as either a client (by supplying "-i ip" of the server)
or as a server (by omitting "-i ip").


# TIPS

Here are some tips to remind myself how I develop this.
I have a Windows laptop, but I do most of my work on Linux.
I use WSL2 on Windows to do the Linux side
and Visual Studio 2022 on the Windows side.

If anybody has a better workflow for a small, multi-platform project,
I would love to learn about it.

To build:
````
$ ./bld.sh
````

After compiling the Linux executables into the bin/Linux directory,
bld.sh copies the files to the Windows partition to a directory
where I do Windows builds.
The "bld.sh" script then prints some instructions and
waits for you to hit return.
Those instructions tell how to launch Visual Studio
and build the programs.

After hitting return, the executables are copied
to the bin/Windows directory.

To check in:
````
$ git add .
$ git status
$ git commit -m "commit message"
$ git push
````


# LICENSE

I want there to be NO barriers to using this code, so I am releasing it to the public domain.  But "public domain" does not have an internationally agreed upon definition, so I use CC0:

This work is dedicated to the public domain under CC0 1.0 Universal:
http://creativecommons.org/publicdomain/zero/1.0/

To the extent possible under law, Steven Ford has waived all copyright
and related or neighboring rights to this work. In other words, you can 
use this code for any purpose without any restrictions.
This work is published from: United States.
Project home: https://github.com/fordsfords/tcp_test
