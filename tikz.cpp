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
	: draw( o ), last_x( 0 ), last_y( 0 )
{
}

////////////////////////////////////////

draw_tikz::~draw_tikz( void )
{
}

////////////////////////////////////////

void draw_tikz::begin( const string &title )
{
	out <<
		"\\begin{figure}[H]\n"
		"\\caption{" << escape( title ) << "}\n"
		"\\label{fig:picture}\n"
		"\\center\n"
		"\\begin{tikzpicture}[yscale=-1]\n";
}

////////////////////////////////////////

void draw_tikz::end( void )
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
	out << "  " << clname(cl) << " (" << em(xx(x)) << "em," << em(yy(y)) << "em) rectangle (" << em(xx(x+w)) << "em," << em(yy(y+h)) << "em);\n";
}

////////////////////////////////////////

void draw_tikz::circle( float x, float y, float r, Class cl )
{
	out << "  " << clname( cl ) << " (" << em(xx(x)) << "em," << em(yy(y)) << "em) circle (" << em(r) << "em);\n";
}

////////////////////////////////////////

void draw_tikz::round( float x, float y, float w, float h, Class cl )
{
	out << "  " << clname( cl ) << "[rounded corners = " << em(h/2.F) << "em] (" << em(xx(x)) << "em," << em(yy(y)) << "em) rectangle (" << em(xx(x+w)) << "em," << em(yy(y+h)) << "em);\n";
}

////////////////////////////////////////

void draw_tikz::text( float x, float y, float w, float h, const string &text, Class cl )
{
	out << "  " << clname( cl ) << " (" << em(xx(x+w/2.F)) << "em," << em(yy(y+h/2.F))+0.25F << "em) node {" << escape( text ) << "};\n";
}

////////////////////////////////////////

void draw_tikz::text_center( float x, float y, float w, float h, const string &text, Class cl )
{
	out << "  " << clname( cl ) << " (" << em(xx(x+w/2.F)) << "em," << em(yy(y+h/2.F))+0.25F << "em) node {" << escape( text ) << "};\n";
}

////////////////////////////////////////

void draw_tikz::path_begin( float x, float y, Class cl )
{
	last_x = x; last_y = y;
	out << "  " << clname( cl ) << "(" << em(xx(x)) << "em," << em(yy(y)) << "em)";
}

////////////////////////////////////////

void draw_tikz::path_h_by( float x )
{
	last_x += x;
	out << " -- ++(" << em(x) << "em,0em)";
}

////////////////////////////////////////

void draw_tikz::path_v_by( float y )
{
	last_y += y;
	out << " -- ++(0em," << em(y) << "em)";
}

////////////////////////////////////////

void draw_tikz::path_h_to( float x )
{
	last_x = x;
	out << " -- (" << em(xx(x)) << "em," << em(yy(last_y)) << "em)";
}

////////////////////////////////////////

void draw_tikz::path_v_to( float y )
{
	last_y = y;
	out << " -- (" << em(xx(last_x)) << "em," << em(yy(y)) << "em)";
}

////////////////////////////////////////

void draw_tikz::path_to( float x, float y )
{
	last_x = x; last_y = y;
	out << " -- (" << em(xx(x)) << "em," << em(yy(y)) << "em)";
}

////////////////////////////////////////

void draw_tikz::path_arc( float r, Arc a )
{
	switch ( a )
	{
		case RIGHT_UP: out << " arc (90:0:" << em(r) << "em)"; last_x += r, last_y -= r;break;
		case RIGHT_DOWN: out << " arc (-90:0:" << em(r) << "em)"; last_x += r, last_y += r;break;
		case LEFT_UP: out << " arc (90:180:" << em(r) << "em)"; last_x -= r, last_y -= r;break;
		case LEFT_DOWN: out << " arc (270:180:" << em(r) << "em)"; last_x -= r, last_y += r;break;
		case UP_RIGHT: out << " arc (180:270:" << em(r) << "em)"; last_x += r, last_y -= r;break;
		case UP_LEFT: out << " arc (0:-90:" << em(r) << "em)"; last_x -= r, last_y -= r;break;
		case DOWN_RIGHT: out << " arc (180:90:" << em(r) << "em)"; last_x -= r, last_y += r;break;
		case DOWN_LEFT: out << " arc (0:90:" << em(r) << "em)"; last_x += r, last_y += r;break;
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
			case '_':
				ret.append( "\\_" );
				break;
			case '&':
				ret.append( "\\&" );
				break;
			case '%':
				ret.append( "\\%" );
				break;
			case '#':
				ret.append( "\\#" );
				break;
			case '$':
				ret.append( "\\$" );
				break;
			case '~':
				ret.append( "\\textasciitilde" );
				break;
			case '^':
				ret.append( "\textasciicircum" );
				break;
			case '\\':
				ret.append( "\\textbackslash" );
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
		case LINE: return "\\draw[line]"; break;
		case ARROW: return "\\fill[arrow]";
		case BOX: return "\\draw[box]"; break;
		case TITLE: return "\\draw[title]"; break;
		case PRODUCTION: return "\\draw[production]"; break;
		case NONTERM: return "\\draw[nonterm]"; break;
		case LITERAL: return "\\draw[literal]"; break;
		case IDENTIFIER: return "\\draw[identifier]"; break;
		case KEYWORD: return "\\draw[keyword]"; break;
		case END: return "\\fill[end]";
		case TEST: return "\\draw[test]"; break;
		default: return "\\draw"; break;
	}
}	

////////////////////////////////////////

float draw_tikz::em( float x )
{
	return x / 24.F;
}

////////////////////////////////////////

