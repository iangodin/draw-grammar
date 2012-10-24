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
#include <string>
#include <vector>
#include <stdexcept>

#include "draw.h"

using namespace std;

////////////////////////////////////////

string escape( const string &t )
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

vector<float> dx( 1, 0.F );
vector<float> dy( 1, 0.F );

float xx( float x ) { return x - dx.back(); }
float yy( float y ) { return y - dy.back(); }

////////////////////////////////////////

void push_translate( const point &p )
{
	dx.push_back( dx.back() - p.x );
	dy.push_back( dy.back() - p.y );
}

void pop_translate( void )
{
	if ( dx.empty() || dy.empty() )
		throw runtime_error( "empty stack" );
	dx.pop_back();
	dy.pop_back();
}

////////////////////////////////////////

void id_begin( ostream &out, float x, float y, float w, float h, const string &name )
{
	out << "<div>";
	out << "<a name=\"" << name << "\">\n";
	out << "  <svg overflow=\"visible\" "
		"xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" "
		"xmlns:svg=\"http://www.w3.org/2000/svg\" "
		"xmlns:xlink=\"http://www.w3.org/1999/xlink\" "
		"width=\"" << w << "px\" height=\"" << h << "px\">\n";
	dx.push_back( dx.back() + x );
	dy.push_back( dy.back() + y );
}

////////////////////////////////////////

void id_end( ostream &out )
{
	dx.pop_back();
	dy.pop_back();
	out << "  </svg>\n";
	out << "</a>";
	out << "</div>\n";
}

////////////////////////////////////////

void link_begin( ostream &out, const string &name )
{
	out << "  <a xlink:href=\"#" << name << "\">\n";
}

////////////////////////////////////////

void link_end( ostream &out )
{
	out << "  </a>\n";
}

////////////////////////////////////////

void box( ostream &out, float x, float y, float w, float h, const string &cl )
{
	out << "  <rect x=\"" << xx(x+0.5F) << "\" y=\"" << yy(y+0.5F) << "\" width=\"" << w << "\" height=\"" << h << "\" class=\"" << cl << "\"></rect>\n";
}

////////////////////////////////////////

void circle( ostream &out, float x, float y, float r, const string &cl )
{
	out << "  <circle cx=\"" << xx(x+0.5F) << "\" cy=\"" << yy(y+0.5F) << "\" r=\"" << r << "\" class=\"" << cl << "\" />\n";
}

////////////////////////////////////////

void round( ostream &out, float x, float y, float w, float h, const string &cl )
{
	out << "  <rect rx=\"" << h/2.F << "\" ry=\"" << h/2.F << "\" x=\"" << xx(x + 0.5F) << "\" y=\"" << yy(y + 0.5F) << "\" width=\"" << w << "\" height=\"" << h << "\" class=\"" << cl << "\"></rect>\n";
}

////////////////////////////////////////

void text( ostream &out, float x, float y, float w, float h, const string &text, const string &cl )
{
	out << "  <text x=\"" << xx(x) << "\" y=\"" << yy(y + h) << "\" class=\"" << cl << "\">";
	out << escape( text ) << "</text>\n";
}

////////////////////////////////////////

void text_center( ostream &out, float x, float y, float w, float h, const string &text, const string &cl )
{
	out << "  <text x=\"" << xx(x + w/2.F) << "\" y=\"" << yy(y + h) << "\" text-anchor=\"middle\" class=\"" << cl << "\">";
	out << escape( text ) << "</text>\n";
}

////////////////////////////////////////

void hline( ostream &out, const point &p1, const point &p2, const string &cl )
{
	path_begin( out, p1.x, p1.y );
	path_h_to( out, p2.x );
	path_end( out, cl );
}

////////////////////////////////////////

void vline( ostream &out, const point &p1, const point &p2, const string &cl )
{
	path_begin( out, p1.x, p1.y );
	path_v_to( out, p2.y );
	path_end( out, cl );
}

////////////////////////////////////////

void path( ostream &out, const point &p1, const point &p2, float r,  Arc dir, const string &cl )
{
	path_begin( out, p1.x, p1.y );
	switch ( dir )
	{
		case RIGHT_UP:
		case RIGHT_DOWN:
			path_h_to( out, p2.x - r );
			break;

		case LEFT_UP:
		case LEFT_DOWN:
			path_h_to( out, p2.x + r );
			break;

		case UP_RIGHT:
		case UP_LEFT:
			path_v_to( out, p2.y + r );
			break;

		case DOWN_RIGHT:
		case DOWN_LEFT:
			path_v_to( out, p2.y - r );
			break;
	}

	path_arc( out, r, dir );
	switch ( dir )
	{
		case RIGHT_UP:
		case RIGHT_DOWN:
		case LEFT_UP:
		case LEFT_DOWN:
			path_v_to( out, p2.y );
			break;

		case UP_RIGHT:
		case UP_LEFT:
		case DOWN_RIGHT:
		case DOWN_LEFT:
			path_h_to( out, p2.x );
			break;
	}
	path_end( out, cl );
}

////////////////////////////////////////

void path( ostream &out, Direction d1, const point &p1, const point &p2, Direction d2, float r, const string &cl )
{
	path_begin( out, p1.x, p1.y );

	switch ( d1 )
	{
		case RIGHT:
			switch ( d2 )
			{
				case RIGHT:
					if ( p1.x < p2.x )
					{
						if ( p1.y < p2.y )
						{
							path_arc( out, r, RIGHT_DOWN );
							path_v_to( out, p2.y - r );
							path_arc( out, r, DOWN_RIGHT );
							path_h_to( out, p2.x );
						}
						else
						{
							path_h_to( out, p2.x - r*2 );
							path_arc( out, r, RIGHT_UP );
							path_v_to( out, p2.y + r );
							path_arc( out, r, UP_RIGHT );
						}
					}
					break;

				case LEFT:
					path_h_to( out, std::max( p1.x, p2.x ) );
					if ( p1.y < p2.y )
					{
						path_arc( out, r, RIGHT_DOWN );
						path_v_to( out, p2.y - r );
						path_arc( out, r, DOWN_LEFT );
						path_h_to( out, p2.x );
					}
					else
					{
						path_h_to( out, std::max( p1.x, p2.x ) );
						path_arc( out, r, RIGHT_UP );
						path_v_to( out, p2.y + r );
						path_arc( out, r, UP_LEFT );
						path_h_to( out, p2.x );
					}
					break;

				case UP:
					path_h_to( out, p2.x - r );
					path_arc( out, r, RIGHT_UP );
					path_v_to( out, p2.y );
					break;

				case DOWN:
					path_h_to( out, p2.x - r );
					path_arc( out, r, RIGHT_DOWN );
					path_v_to( out, p2.y );
					break;


				default:
					break;
			}
			break;

		case LEFT:
			switch ( d2 )
			{
				case RIGHT:
					path_h_to( out, std::min( p1.x, p2.x ) );
					if ( p1.y < p2.y )
					{
						path_arc( out, r, LEFT_DOWN );
						path_v_to( out, p2.y - r );
						path_arc( out, r, DOWN_RIGHT );
						path_h_to( out, p2.x );
					}
					else
					{
						path_arc( out, r, LEFT_UP );
						path_v_to( out, p2.y + r );
						path_arc( out, r, UP_RIGHT );
						path_h_to( out, p2.x );
					}
					break;

				case LEFT:
					if ( p1.x < p2.x )
					{
						if ( p1.y < p2.y )
						{
							path_arc( out, r, LEFT_DOWN );
							path_v_to( out, p2.y - r );
							path_arc( out, r, DOWN_LEFT );
							path_h_to( out, p2.x );
						}
						else
						{
							path_h_to( out, p2.x - r*2 );
							path_arc( out, r, LEFT_UP );
							path_v_to( out, p2.y + r );
							path_arc( out, r, UP_LEFT );
						}
					}
					else
					{
						if ( p1.y < p2.y )
						{
							path_arc( out, r, LEFT_DOWN );
							path_v_to( out, p2.y - r );
							path_arc( out, r, DOWN_LEFT );
							path_h_to( out, p2.x );
						}
						else
						{
							path_h_to( out, p2.x + r*2 );
							path_arc( out, r, LEFT_UP );
							path_v_to( out, p2.y + r );
							path_arc( out, r, UP_LEFT );
						}
					}
					break;

				case UP:
					path_h_to( out, p2.x - r );
					path_arc( out, r, LEFT_UP );
					path_v_to( out, p2.y );
					break;

				case DOWN:
					path_h_to( out, p2.x + r );
					path_arc( out, r, LEFT_DOWN );
					path_v_to( out, p2.y );
					break;
			}
			break;

		case UP:
			switch ( d2 )
			{
				case RIGHT:
					path_v_to( out, p2.y + r );
					path_arc( out, r, UP_RIGHT );
					path_h_to( out, p2.x );
					break;

				default:
					throw runtime_error( "Not yet implemented" );
			}
			break;

		case DOWN:
			switch ( d2 )
			{
				case RIGHT:
					path_v_to( out, p2.y - r );
					path_arc( out, r, DOWN_RIGHT );
					path_h_to( out, p2.x );
					break;

				case LEFT:
					path_v_to( out, p2.y - r );
					path_arc( out, r, DOWN_LEFT );
					path_h_to( out, p2.x );
					break;

				case UP:
					path_v_to( out, std::max( p1.y, p2.y ) );
					if ( p2.x < p1.x )
					{
						path_arc( out, r, DOWN_LEFT );
						path_h_to( out, p2.x+r );
						path_arc( out, r, LEFT_UP );
					}
					else
					{
						path_arc( out, r, DOWN_RIGHT );
						path_h_to( out, p2.x-r );
						path_arc( out, r, RIGHT_UP );
					}
					path_v_to( out, p2.y );
					break;

				case DOWN:
					path_v_to( out, std::max( p1.y, p2.y ) - r*2 );
					if ( p2.x < p1.x )
					{
						path_arc( out, r, DOWN_LEFT );
						path_h_to( out, p2.x+r );
						path_arc( out, r, LEFT_DOWN );
					}
					else
					{
						path_arc( out, r, DOWN_RIGHT );
						path_h_to( out, p2.x-r );
						path_arc( out, r, RIGHT_DOWN );
					}
					break;

			}
			break;

		default:
			break;
	}

	path_end( out, cl );
}

////////////////////////////////////////

void arrow_left( ostream &out, const point &p, float l, float size, const string &cl1, const string &cl2 )
{
	if ( l > size/2 )
	{
		path_begin( out, p.x, p.y );
		out << " h " << -l + size/2;
		path_end( out, cl1 );
	}

	path_begin( out, p.x - l, p.y );
	path_arrow_left( out, size );
	path_end( out, cl2 );
}

////////////////////////////////////////

void arrow_right( ostream &out, const point &p, float l, float size, const string &cl1, const string &cl2 )
{
	if ( l > size/2 )
	{
		path_begin( out, p.x, p.y );
		out << " h " << l - size/2;
		path_end( out, cl1 );
	}

	path_begin( out, p.x + l, p.y );
	path_arrow_right( out, size );
	path_end( out, cl2 );
}

////////////////////////////////////////

void arrow_down( ostream &out, const point &p, float l, float size, const string &cl1, const string &cl2 )
{
	if ( l > size/2 )
	{
		path_begin( out, p.x, p.y );
		out << " v " << l - size/2;
		path_end( out, cl1 );
	}

	path_begin( out, p.x, p.y + l );
	path_arrow_down( out, size );
	path_end( out, cl2 );
}

////////////////////////////////////////

void path_begin( ostream &out, float x, float y )
{
	out << "  <path d=\"M " << xx(x) << ' ' << yy(y);
}

////////////////////////////////////////

void path_h_to( ostream &out, float x )
{
	out << " H " << xx(x);
}

////////////////////////////////////////

void path_v_to( ostream &out, float y )
{
	out << " V " << yy(y);
}

////////////////////////////////////////

void path_line_to( ostream &out, float x, float y )
{
	out << " L " << xx(x) << ' ' << yy(y);
}

////////////////////////////////////////

void path_arc( ostream &out, float r, Arc a )
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

void path_arrow_left( ostream &out, float size )
{
	out << " l " << size << ' ' << size/2 << " 0 " << -size << " z";
}

////////////////////////////////////////

void path_arrow_right( ostream &out, float size )
{
	out << " l " << -size << ' ' << -size/2 << " 0 " << size << ' ' << size << ' ' << -size/2 << " z";
}

////////////////////////////////////////

void path_arrow_down( ostream &out, float size )
{
	out << " l " << -size/2 << ' ' << -size << ' ' << size << " 0 " << " z";
}

////////////////////////////////////////

void path_end( ostream &out, const string &cl )
{
	out <<  "\" class=\"" << cl << "\"/>\n";
}

////////////////////////////////////////

