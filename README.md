# tcp_test
TCP server and client portable across Mac, Linux, Windows.

# Table of contents

<sup>(table of contents from https://luciopaiva.com/markdown-toc/)</sup>


# INTRODUCTION

This repository is for misc TCP test tools.
At the present time, it is primarily for testing TCP's keepalive feature.

The approach is simple:
* Two programs, "tcp_send" and "tcp_rcv".
* Either one can function as a server or client.
The client initiates a connection to the server.
* Either or both can enable TCP keepalives.
* The sender waits for the connetion to be established.
* The sender does the following:
````
1. Send first message.
2. Pause for "-s sleeptime" seconds (usually many minutes).
3. Send a specified number of additional messages, one per second.
````

During the step 2's sleep,
the connection can be examined using a packet capture tool (like Wireshark)
to verify the presence or absense of keepalives.

# STATEFUL ROUTER TIMING OUT IDLE CONNECTIONS

A good use case for these tools are to determine if an intervening
router between two hosts is maintaining state for TCP connections,
and if so, if it fimes out idle connections.

This is common for routers that implment
[NAT (Network Address Translation)](https://en.wikipedia.org/wiki/Network_address_translation).
TCP connections consume router resources and must be cleaned up
to prevent resource exhaustion.

Here's a test to see if the router between two hosts is timing out connetions:

Host1 (ip address 10.29.3.10):
````
tcp_rcv -p 12000 -o rcv.txt
````

Host2:
````
tcp_send -p 12000 -i 10.29.3.10 -n 2000 -s 3600 -o send.txt
````

The "-s 3600" causes tcp_send to sleep for 3600 seconds (1 hour) after sending the
first message.
After that, it sends 2000 messages at a rate of 1 message per second.
If the router between the hosts maintains TCP connection state and has timed
out the idle connection during the 1 hour sleep,
you will see the tcp_send start sending again, but the tcp_rcv will not
receive any messages.
The tcp_send tool will continue writing messages to its socket for about 16 minutes
(900+ messages), and then time out the connection.

On the other hand, if the router does not time out the connection,
the tcp_rcv sill get the messages sent after the 1-hour sleep.

To verify that it is related to lack of traffic on the connection,
repeat the test adding "-k 10" to both commands.
This ensures that during idle periods,
TCP keepalive packets will be sent every 10 seconds.

# HELP

````
Usage: tcp_send [-h] [-i ip] [-k keepalive] [-l lingertime] [-n num_msgs] [-p port] [-s sleeptime]
where:
  -h : print help.
  -i ip : IP address to connect to (sets client mode).
  -k keepalive : Seconds between TCP keepalive probes (0=none).
  -l lingertime : Seconds to sleep (after last send) before shutting down.
  -n num_msgs : number of hello world messagse to send.
  -o outfile : Program output will be written to screen and this file.
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
  -o outfile : Program output will be written to screen and this file.
  -p port : listener port.
````

Note that tcp_rcv can be run as either a client (by supplying "-i ip" of the server)
or as a server (by omitting "-i ip").


# TIPS

Some tips to remind myself how I develop this.
I have a Windows laptop, but I do most of my work on Linux.
I use WSL2 on Windows to do the Linux side,
and Visual Studio 2022 on the Windows side.

These steps probably won't work for anybody but me.
If anybody has a better workflow for a small multi-platform project,
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

Copyright 2021-2022 Steven Ford http://geeky-boy.com and licensed
"public domain" style under
[CC0](http://creativecommons.org/publicdomain/zero/1.0/):
![CC0](https://licensebuttons.net/p/zero/1.0/88x31.png "CC0")

To the extent possible under law, the contributors to this project have
waived all copyright and related or neighboring rights to this work.
In other words, you can use this code for any purpose without any
restrictions.  This work is published from: United States.  The project home
is https://github.com/fordsfords/tcp_test

To contact me, Steve Ford, project owner, you can find my email address
at http://geeky-boy.com.  Can't see it?  Keep looking.
