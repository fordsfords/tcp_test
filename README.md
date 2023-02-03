# tcp_test
TCP server and client portable across Mac, Linux, Windows.

# Table of contents

<sup>(table of contents from https://luciopaiva.com/markdown-toc/)</sup>


# INTRODUCTION

This package is for misc TCP test tools. Over time, it may become quite a hodge-podge.
At the present time, it is only for testing TCP's keepalive feature.


# HELP

````
Usage: tcp_send [-h] [-i ip] [-k keepalive] [-l lingertime] [-n num_msgs] [-p port] [-s sleeptime]
where:
  -h : print help.
  -i ip : IP address to connect to (sets client mode).
  -k keepalive : Seconds between TCP keepalive probes (0=none).
  -l lingertime : Seconds to sleep (after last send) before shutting down.
  -n num_msgs : number of hello world messagse to send.
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

Some tips to remind myself how I develop this.
I have a Windows laptop, but I do most of my work on Linux.
I use WSL2 on Windows to do the Linux side,
and Visual Studio 2022 on the Windows side.

These steps probably won't work for anybody but me.
If anybody has a better workflow for a small multi-platform project,
I would love to learn about it.

From Unix shell window, build Unix:
````
$ ./bld.sh
````

From Unix shell window, start process of building Windows:
````
$ cp *.* /mnt/c/Users/sford/tstvs/tcp_test/
````

From Windows command prompt:
````
> cd \Users\sford\tstvs\tcp_test
> tcp_test.sln
````
This brings up visual studio.
* Change "Debug" to "Release", change "Win32" to "x64".
* Right-click solution, clean.
* Right-click "tcp_rcv", "build" (not "rebuild").
* Right-click "tcp_send", "build" (not "rebuild").
* Exit Visual Studio

From Unix shell window:
````
$ cp /mnt/c/Users/sford/tstvs/tcp_test/*.sln /mnt/c/Users/sford/tstvs/tcp_test/*.vcxproj .
$ cp /mnt/c/Users/sford/tstvs/tcp_test/x64/Release/*.exe bin/Windows/
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
