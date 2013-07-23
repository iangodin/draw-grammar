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

using namespace std;

////////////////////////////////////////

draw_svg::draw_svg( ostream &o )
	: draw( o )
{
}

////////////////////////////////////////

draw_svg::~draw_svg( void )
{
}

////////////////////////////////////////

void draw_svg::begin( const string &title )
{
}

////////////////////////////////////////

void draw_svg::end( void )
{
}

////////////////////////////////////////

void draw_svg::id_begin( float x, float y, float w, float h, const string &name )
{
	out <<
		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		"<?xml-stylesheet href=\"svg.css\" type=\"text/css\"?>\n"
		"<svg overflow=\"visible\" "
		"xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" "
		"xmlns:svg=\"http://www.w3.org/2000/svg\" "
		"xmlns:xlink=\"http://www.w3.org/1999/xlink\" "
		"width=\"" << w << "px\" height=\"" << h << "px\">\n";
	push_translate( point( x, y ) );
}

////////////////////////////////////////

void draw_svg::id_end( void )
{
	pop_translate();
	out << "</svg>\n";
}

////////////////////////////////////////

void draw_svg::link_begin( const string &name )
{
	out << "  <a xlink:href=\"#" << name << "\">\n";
}

////////////////////////////////////////

void draw_svg::link_end( void )
{
	out << "  </a>\n";
}

////////////////////////////////////////

void draw_svg::box( float x, float y, float w, float h, Class cl )
{
	out << "  <rect x=\"" << xx(x+0.5F) << "\" y=\"" << yy(y+0.5F) << "\" width=\"" << w << "\" height=\"" << h << "\" class=" << clname( cl ) << "></rect>\n";
}

////////////////////////////////////////

void draw_svg::circle( float x, float y, float r, Class cl )
{
	out << "  <circle cx=\"" << xx(x+0.5F) << "\" cy=\"" << yy(y+0.5F) << "\" r=\"" << r << "\" class=" << clname( cl ) << " />\n";
}

////////////////////////////////////////

void draw_svg::round( float x, float y, float w, float h, Class cl )
{
	out << "  <rect rx=\"" << h/2.F << "\" ry=\"" << h/2.F << "\" x=\"" << xx(x + 0.5F) << "\" y=\"" << yy(y + 0.5F) << "\" width=\"" << w << "\" height=\"" << h << "\" class=" << clname( cl ) << "></rect>\n";
}

////////////////////////////////////////

void draw_svg::text( float x, float y, float w, float h, const string &text, Class cl )
{
	out << "  <text x=\"" << xx(x) << "\" y=\"" << yy(y + h/2.F) << "\" alignment-baseline=\"central\" class=" << clname( cl, true ) << ">";
	out << escape( text ) << "</text>\n";
}

////////////////////////////////////////

void draw_svg::text_center( float x, float y, float w, float h, const string &text, Class cl )
{
	out << "  <text x=\"" << xx(x + w/2.F) << "\" y=\"" << yy(y + h/2.F) << "\" text-anchor=\"middle\" alignment-baseline=\"central\" class=" << clname( cl, true ) << ">";
	out << escape( text ) << "</text>\n";
}

////////////////////////////////////////

void draw_svg::path_begin( float x, float y, Class cl )
{
	out << "  <path class=" << clname( cl ) << " d=\"M " << xx(x) << ' ' << yy(y);
}

////////////////////////////////////////

void draw_svg::path_h_by( float x )
{
	out << " h " << x;
}

////////////////////////////////////////

void draw_svg::path_v_by( float y )
{
	out << " v " << y;
}

////////////////////////////////////////

void draw_svg::path_h_to( float x )
{
	out << " H " << xx(x);
}

////////////////////////////////////////

void draw_svg::path_v_to( float y )
{
	out << " V " << yy(y);
}

////////////////////////////////////////

void draw_svg::path_to( float x, float y )
{
	out << " L " << xx(x) << ' ' << yy(y);
}

////////////////////////////////////////

void draw_svg::path_arc( float r, Arc a )
{
	switch ( a )
	{
		case RIGHT_UP: out << " a " << r << ' ' << r << " 0 0 0 " << r << ' ' << -r; break;
		case RIGHT_DOWN: out << " a " << r << ' ' << r << " 0 0 1 " << r << ' ' << r; break;
		case LEFT_UP: out << " a " << r << ' ' << r << " 0 0 1 " << -r << ' ' << -r; break;
		case LEFT_DOWN: out << " a " << r << ' ' << r << " 0 0 0 " << -r << ' ' << r; break;
		case UP_RIGHT: out << " a " << r << ' ' << r << " 0 0 1 " << r << ' ' << -r; break;
		case UP_LEFT: out << " a " << r << ' ' << r << " 0 0 0 " << -r << ' ' << -r; break;
		case DOWN_RIGHT: out << " a " << r << ' ' << r << " 0 0 0 " << r << ' ' << r; break;
		case DOWN_LEFT: out << " a " << r << ' ' << r << " 0 0 1 " << -r << ' ' << r; break;
	}
}

////////////////////////////////////////

void draw_svg::path_arrow_left( float size )
{
	out << " l " << size << ' ' << size/2 << " 0 " << -size << " z";
}

////////////////////////////////////////

void draw_svg::path_arrow_right( float size )
{
	out << " l " << -size << ' ' << -size/2 << " 0 " << size << ' ' << size << ' ' << -size/2 << " z";
}

////////////////////////////////////////

void draw_svg::path_arrow_down( float size )
{
	out << " l " << -size/2 << ' ' << -size << ' ' << size << " 0 " << " z";
}

////////////////////////////////////////

void draw_svg::path_end( void )
{
	out << "\"/>\n";
}

////////////////////////////////////////

string draw_svg::escape( const string &t )
{
	string ret;
	for ( size_t i = 0; i < t.size(); ++i )
	{
		switch ( t[i] )
		{
			case '<':
				ret.append( "&lt;" );
				break;
			case '>':
				ret.append( "&gt;" );
				break;
			default:
				ret.push_back( t[i] );
				break;
		}
	}

	return ret;
}

////////////////////////////////////////

string draw_svg::clname( Class cl, bool text )
{
	if ( text )
	{
		std::string ret;
		switch ( cl )
		{
			case LINE: ret = "\"line\" fill=\"black\""; break;
			case ARROW: ret = "\"arrow\" fill=\"black\""; break;
			case BOX: ret = "\"box\" fill=\"black\""; break;
			case TITLE: ret = "\"title\" fill=\"black\""; break;
			case PRODUCTION: ret = "\"prod\" fill=\"black\""; break;
			case NONTERM: ret = "\"nonterm\" fill=\"red\""; break;
			case LITERAL: ret = "\"literal\" fill=\"green\""; break;
			case IDENTIFIER: ret = "\"ident\" fill=\"black\""; break;
			case KEYWORD: ret = "\"keyword\" fill=\"blue\""; break;
			case END: ret = "\"end\" fill=\"black\""; break;
			case TEST: ret = "\"test\" fill=\"black\""; break;
		}
		if ( ret.empty() )
			return "\"unknown\"";
		return ret + " stroke=\"none\" font-family=\"Courier,monospace\" font-weight=\"bold\" font-size=\"20px\"";
	}
	else
	{
		switch ( cl )
		{
			case LINE: return "\"line\" stoke-width=\"2px\" stroke=\"black\" fill=\"none\" stroke-width=\"2px\"";
			case ARROW: return "\"arrow\" stroke=\"none\" fill=\"black\" stroke-width=\"2px\"";
			case BOX: return "\"box\" stroke=\"black\" fill=\"none\" stroke-width=\"2px\"";
			case TITLE: return "\"title\" stroke=\"black\" fill=\"none\" stroke-width=\"2px\"";
			case PRODUCTION: return "\"prod\" stroke=\"black\" fill=\"none\" stroke-width=\"2px\"";
			case NONTERM: return "\"nonterm\" stroke=\"black\" fill=\"none\" stroke-width=\"2px\"";
			case LITERAL: return "\"literal\" stroke=\"black\" fill=\"none\" stroke-width=\"2px\"";
			case IDENTIFIER: return "\"ident\" stroke=\"black\" fill=\"none\" stroke-width=\"2px\"";
			case KEYWORD: return "\"keyword\" stroke=\"black\" fill=\"none\" stroke-width=\"2px\"";
			case END: return "\"end\" stroke=\"black\" fill=\"none\" stroke-width=\"2px\"";
			case TEST: return "\"test\" stroke=\"black\" fill=\"none\"";
		}
	}

	return "\"unknown\"";
}

////////////////////////////////////////

