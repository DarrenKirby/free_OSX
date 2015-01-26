# free_OSX
An implementation of the venerable Unix 'free' command for OS X

FAQ:

Q. Where's the +/- buffers/cached? shared?

A. I am not sure the mach kernel computes this information. If it
does I have not found how to access it programatically. Feel free
to drop me a line if you know how and I'll add it.

Q. How do I build/install this?

A. Pretty simple. just run 'make' in the directory with the .c and
.h file. You will need a compiler (gcc, clang) installed. This will
build the 'free' binary. Copy that somewhere. /usr/local/bin is a
good choice. Then copy the manpage somewhere. /usr/local/share/man/man1
is a good choice.

