Intro
=====
Newtstring is a tiny C library for reading and writing a modified form of 
[netstrings] [djb_netstring].

Format
======
The Newtstring format is identical to the [netstring] [djb_netstring] format 
except for two minor changes:
1. The length prefix is in hexadecimal. This makes it easy to check for 
overflows of the platform's size type, and makes parsing and validation much 
easier if the header has to be a fixed length.
2. The trailing character is a newline (LF) instead of a comma. This makes logs 
easier to read, since every new message starts on a new line, and plays nicely 
with most terminals' line-buffering scheme.

  [djb_netstring]: http://cr.yp.to/proto/netstrings.txt

Interface
=========
The Newtstring library is designed to send arbitrary data buffers, not just 
strings, so library functions take a buffer object, `struct msg`, which holds a 
buffer and a length. A `msg` object is considered invalid when the buffer is 
`NULL` and the length is non-zero. This is a weird way to indicate errors, but 
a zero-length message is valid and may have a NULL buffer pointer (it's up to 
`malloc`).

The function `mk_msg` takes a data buffer (`msg`) in as a parameter and returns 
a new `msg` with the buffer holding the newtstring (note that this does not 
include a trailing NUL byte). `mk_msg` will return an invalid `msg` object if 
creating a newtstring would overflow the platform's `size_t` type.

`mk_msg`'s companion function, `read_msg`, takes a `msg` object holding a 
newtstring and returns a `msg` object holding the newtstring's data. `read_msg` 
will return an invalid `msg` if the header would overflow the platform's 
`size_t` type, if there is a non-hex-digit character in the header, or if the 
header describes a message too large to fit in the input `msg`.
