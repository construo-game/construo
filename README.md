Construo
========

Construo is a programm for constructing wire-frame objects and let
them react to physical forces, it is neither a real simulation nor a
real game, it is just a little toy app which can be a nice way to
waste time. Some game-like elements might follow later, see the file
`TODO`. Construo runs under both OpenGL and plain Xlib.

The Construo webpage is located at:

* http://construo.github.io/

and the source repository is located at:

* http://github.com/Construo/construo

For questions, comments, constructions, bugs reports and other things
write a mail to:

* Ingo Ruhnke <grumbel@gmail.com>

or send it directly to the Construo Development mailing list at:

* Construo Development List <construo-devel@nongnu.org>


Compilation
-----------

Construo can be compiled with:

    mkdir build
    cd build
    cmake ..


Running
-------

Construo comes in two binaries `construo.glut` and `construo.x11`. The
former uses OpenGL and the later software rendering with X11. To run it:

    build/construo.glut
or

    build/construo.x11

