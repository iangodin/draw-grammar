draw-grammar
============

Draw grammar as SVG, HTML, or TeX

Supported OS
------------

Compiles and runs on Linux, and OS X.  I am using Ubuntu 12.4 for Linux, and Mountain Lion for OS X.
A compiler supporting C++11 is needed (g++ 4.6 or better, or clang 4.0 or better).

Requirements
------------

On Linux, you will need lua and lua-posix (packages are usually available for Linux, look for lua and liblua-posix).
Ninja is used as the build tool.
DParser is used as the parsing tool for the grammar file.

Here are the location for the required tools:
 * http://lua.org
 * http://files.luaforge.net/releases/luaposix/luaposix
 * http://martine.github.com/ninja/
 * http://dparser.sourceforge.net/

Building
--------

There is a Makefile provided, but ninja is really used to compile.
Once the requirements are installed, simply run "make" to compile.
There's a single executable in the directory build/bin/draw_grammar

You can also use "make debug" which will create an executable in debug/bin/draw_grammar.

The configure script is in fact a lua script.
You do not need to invoke this script, the Makefile will call configure for you.
It will search for the required libraries and programs, and create a build, release, and debug directory.
There is currently no difference between build and release.

Sample
------

Here is what a sample grammar file looks like:
	"Sample EBNF"
	{
		optional = one | two | three .
		one_or_more = < x ~ "," > .
		zero_or_more = { x } .
	}

And the rendered SVG file:

![Sample Grammar](https://raw.github.com/iangodin/draw-grammar/master/sample.png)
