# free_OSX
An implementation of the venerable Unix 'free' command for OS X

FAQ:

Q. How do I build/install this?

A. Pretty simple. To build the `free` binary just run `make`
in the directory with the .c file. You will need a compiler
installed. The build script uses gcc by default. If it can't
find `gcc` it will use `clang`. You can choose any compiler by
passing a variable like `make CC=clang`.

If you run ` sudo make install` it will copy the binary to
/usr/local/bin and manpage to /usr/local/share/man/man1.

Q. Where's the +/- buffers/cached? shared?

A. I am not sure the mach kernel computes this information. If it
does I have not found how to access it programatically. Feel free
to drop me a line if you know how and I'll add it.
