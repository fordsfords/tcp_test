# tcp_test
TCP server and client portable across Mac, Linux, Windows.

# Table of contents

<sup>(table of contents from https://luciopaiva.com/markdown-toc/)</sup>

# INTRODUCTION

This package is for misc TCP test tools. Over time, it may become quite a hodge-podge.
At the present time, it is only for testing TCP's keepalive feature.

# TIPS

Some tips to remind myself how I develop this.
I have a Windows laptop, but I do most of my work on Linux.
I use WSL2 on Windows to do the Linux side,
and Visual Studio 2022 on the Windows side.

From Unix shell window:
````
$ ./bld.sh
$ cp *.* /mnt/c/Users/sford/tstvs/tcp_test/
````

From Windows command prompt:
````
> cd \Users\sford\tstvs\tcp_test
> tcp_send.vcxproj
````
This brings up visual studio. Build both tools.

From Unix shell window:
````
$ cp /mnt/c/Users/sford/tstvs/tcp_test/*.sln /mnt/c/Users/sford/tstvs/tcp_test/*.vcxproj .
$ cp /mnt/c/Users/sford/tstvs/tcp_test/x64/Debug/*.exe bin/Windows/
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
