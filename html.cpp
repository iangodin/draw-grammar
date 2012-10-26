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
#include <stdexcept>

#include "svg.h"
#include "html.h"

using namespace std;

////////////////////////////////////////

draw_html::draw_html( ostream &o )
	: draw_svg( o )
{
	out <<
		"<html>\n"
		"<head>\n"
		"  <title>Grammar</title>\n"
		"  <meta http-equiv=\"Content-Type\" content=\"application/xhtml+xml; charset=UTF-8\"></meta>\n"
		"  <link type=\"text/css\" rel=\"stylesheet\" href=\"svg.css\"></link>\n"
		"</head>\n"
		"<body>\n";
}

////////////////////////////////////////

draw_html::~draw_html( void )
{
	out << "</body>\n";
	out << "</html>\n";
}

////////////////////////////////////////

void draw_html::id_begin( float x, float y, float w, float h, const string &name )
{
	out << "<div>";
	out << "<a name=\"" << name << "\">\n";
	draw_svg::id_begin( x, y, w, h, name );
}

////////////////////////////////////////

void draw_html::id_end( void )
{
	draw_svg::id_end();
	out << "</a>";
	out << "</div>\n";
}

////////////////////////////////////////

