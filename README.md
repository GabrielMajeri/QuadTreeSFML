# QuadTreeSFML
Quad Tree for collision checking implemented in C++ with SFML 2.

![Screenshot of an example](http://i.imgur.com/1GvnpXn.png)

## Compiling
The source code must be compiled with a C++14 compatible compiler, for example GCC 4.9 or newer, or Visual C++ 2015 or newer.

## Issues
The node is cleared and rebuilt each frame. This is quite inefficient for large number of objects. I am working on a fix.
