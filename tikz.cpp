//
// Copyright (c) 2012 Ian Godin
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#include <iostream>
#include <sstream>
#include <stdexcept>

#include "tikz.h"

using namespace std;

////////////////////////////////////////

draw_tikz::draw_tikz( ostream &o )
	: draw( o )
{
	out <<
		"\\begin{figure}[H]\n"
		"\\caption{Figure}\n"
		"\\label{fig:picture}\n"
		"\\center\n"
		"\\begin{tikzpicture}[yscale=-1]\n";
}

////////////////////////////////////////

draw_tikz::~draw_tikz( void )
{
	out <<
		"\\end{tikzpicture}\n"
		"\\end{figure}\n";
}

////////////////////////////////////////

void draw_tikz::id_begin( float x, float y, float w, float h, const string &name )
{
	push_translate( point( x, y ) );
}

////////////////////////////////////////

void draw_tikz::id_end( void )
{
	pop_translate();
}

////////////////////////////////////////

void draw_tikz::link_begin( const string &name )
{
}

////////////////////////////////////////

void draw_tikz::link_end( void )
{
}

////////////////////////////////////////

void draw_tikz::box( float x, float y, float w, float h, Class cl )
{
	out << "  \\draw (" << em(xx(x)) << "em," << em(yy(y)) << "em) rectangle (" << em(xx(x+w)) << "em," << em(yy(y+h)) << "em);\n";
}

////////////////////////////////////////

void draw_tikz::circle( float x, float y, float r, Class cl )
{
	out << "  " << clname( cl ) << " (" << em(xx(x)) << "em," << em(yy(y)) << "em) circle (" << em(r) << "em);\n";
}

////////////////////////////////////////

void draw_tikz::round( float x, float y, float w, float h, Class cl )
{
	out << "  \\draw [rounded corners = " << em(h/2.F) << "em] (" << em(xx(x)) << "em," << em(yy(y)) << "em) rectangle (" << em(xx(x+w)) << "em," << em(yy(y+h)) << "em);\n";
}

////////////////////////////////////////

void draw_tikz::text( float x, float y, float w, float h, const string &text, Class cl )
{
	out << "  \\draw (" << em(xx(x+w/2.F)) << "em," << em(yy(y+h/2.F)) << "em) node {" << escape( text ) << "};\n";
}

////////////////////////////////////////

void draw_tikz::text_center( float x, float y, float w, float h, const string &text, Class cl )
{
	out << "  \\draw (" << em(xx(x+w/2.F)) << "em," << em(yy(y+h/2.F)) << "em) node {" << escape( text ) << "};\n";
}

////////////////////////////////////////

void draw_tikz::path_begin( float x, float y, Class cl )
{
	out << "  " << clname( cl ) << "(" << em(xx(x)) << "em," << em(yy(y)) << "em)";
}

////////////////////////////////////////

void draw_tikz::path_h_by( float x )
{
	out << " -- ++(" << em(x) << "em,0em)";
}

////////////////////////////////////////

void draw_tikz::path_v_by( float y )
{
	out << " -- ++(0em," << em(y) << "em)";
}

////////////////////////////////////////

void draw_tikz::path_h_to( float x )
{
	out << " svg[scale=1em] \"H " << em(xx(x)) << "\"";
}

////////////////////////////////////////

void draw_tikz::path_v_to( float y )
{
	out << " svg[scale=1em] \"V " << em(yy(y)) << "\"";
}

////////////////////////////////////////

void draw_tikz::path_to( float x, float y )
{
	out << " -- (" << em(xx(x)) << "em," << em(yy(y)) << "em)";
}

////////////////////////////////////////

void draw_tikz::path_arc( float r, Arc a )
{
	switch ( a )
	{
		case RIGHT_UP: out << " arc (90:0:" << em(r) << "em)"; break;
		case RIGHT_DOWN: out << " arc (-90:0:" << em(r) << "em)"; break;
		case LEFT_UP: out << " arc (-90:-180:" << em(r) << "em)"; break;
		case LEFT_DOWN: out << " arc (90:180:" << em(r) << "em)"; break;
		case UP_RIGHT: out << " arc (180:270:" << em(r) << "em)"; break;
		case UP_LEFT: out << " arc (0:-90:" << em(r) << "em)"; break;
		case DOWN_RIGHT: out << " arc (180:90:" << em(r) << "em)"; break;
		case DOWN_LEFT: out << " arc (0:90:" << em(r) << "em)"; break;
	}
}

////////////////////////////////////////

void draw_tikz::path_arrow_left( float size )
{
	out << " -- ++(" << em( size ) << "em," << em( size/2 ) << "em) -- ++(0em," << em(-size) << "em) -- cycle";
}

////////////////////////////////////////

void draw_tikz::path_arrow_right( float size )
{
	out << " -- ++(" << em( -size ) << "em," << em( -size/2 ) << "em) -- ++(0em," << em(size) << "em) -- ++(" << em(size) << "em," << em(-size/2) << "em) -- cycle";
}

////////////////////////////////////////

void draw_tikz::path_arrow_down( float size )
{
	out << " -- ++(" << em(-size/2) << "em," << em(-size) << "em) -- ++(" << em(size) << "em,0em) -- cycle";
}

////////////////////////////////////////

void draw_tikz::path_end( void )
{
	out << ";\n";
}

////////////////////////////////////////

string draw_tikz::escape( const string &t )
{
	string ret;
	for ( size_t i = 0; i < t.size(); ++i )
	{
		switch ( t[i] )
		{
			case '{':
				ret.append( "\\{" );
				break;
			case '}':
				ret.append( "\\}" );
				break;
			case '<':
				ret.append( "<" );
				break;
			case '>':
				ret.append( ">" );
				break;
			case '\\':
				break;
			case '_':
				ret.push_back( ' ' );
				break;
			default:
				ret.push_back( t[i] );
				break;
		}
	}

	return ret;
}

////////////////////////////////////////

string draw_tikz::clname( Class cl )
{
	switch ( cl )
	{
		case END:
		case ARROW:
			  return "\\fill";

		default:
			return "\\draw";
	}

}	

////////////////////////////////////////

float draw_tikz::em( float x )
{
	return x / 24.F;
}

////////////////////////////////////////

