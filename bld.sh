#!/bin/sh

# Update TOC in doc
for F in *.md; do :
  if egrep "<!-- mdtoc-start -->" $F >/dev/null; then :
    # Update doc table of contents (see https://github.com/fordsfords/mdtoc).
    if which mdtoc.pl >/dev/null; then mdtoc.pl -b "" $F;
    elif [ -x ../mdtoc/mdtoc.pl ]; then ../mdtoc/mdtoc.pl -b "" $F;
    else echo "FYI: mdtoc.pl not found; Skipping doc build"; echo ""; fi
  fi
done

OPTS=""
if echo "$OSTYPE" | egrep -i linux >/dev/null; then :
  OPTS="-pthread"
fi

echo "Building code"

gcc -Wall -o bin/Linux/tcp_send $OPTS cprt.c tcp_utils.c tcp_send.c
if [ $? -ne 0 ]; then exit 1; fi

gcc -Wall -o bin/Linux/tcp_rcv $OPTS cprt.c tcp_utils.c tcp_rcv.c
if [ $? -ne 0 ]; then exit 1; fi


ANS=""
while [ "$ANS" != "y" -a "$ANS" != "n" ]; do :
  if [ "$ANS" = "" ]; then :
    echo "Linux build done. Continue with Windows? (y/n)"
  fi
  read ANS
  if [ "$ANS" != "y" -a "$ANS" != "n" ]; then :
    echo "Bad input, respond with y or n"
  fi
done

if [ "$ANS" = "y" ]; then :
  # The following steps are specific to each individual's workflow.
  cp *.* /mnt/c/Users/sford/tstvs/tcp_test/

  cat <<__EOF__
From Windows command prompt:
> cd \Users\sford\tstvs\tcp_test
> tcp_test.sln

This brings up visual studio.
* Change "Debug" to "Release", change "Win32" to "x64".
* Right-click solution, clean.
* Right-click "tcp_rcv", "build" (not "rebuild").
* Right-click "tcp_send", "build" (not "rebuild").
* Exit Visual Studio

Press return when done.
__EOF__
  read ans

  cp /mnt/c/Users/sford/tstvs/tcp_test/*.sln /mnt/c/Users/sford/tstvs/tcp_test/*.vcxproj .
  cp /mnt/c/Users/sford/tstvs/tcp_test/x64/Release/*.exe bin/Windows/

  echo "Done."
fi

echo "Success"
