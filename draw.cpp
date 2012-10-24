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

draw::draw( ostream &o )
	: out( o ), dx( 1, 0.F ), dy( 1, 0.F )
{
}

////////////////////////////////////////

draw::~draw( void )
{
}

////////////////////////////////////////

void draw::push_translate( const point &p )
{
	dx.push_back( dx.back() - p.x );
	dy.push_back( dy.back() - p.y );
}

////////////////////////////////////////

void draw::pop_translate( void )
{
	if ( dx.empty() || dy.empty() )
		throw runtime_error( "empty stack" );
	dx.pop_back();
	dy.pop_back();
}

////////////////////////////////////////

void draw::hline( const point &p1, const point &p2, Class cl )
{
	path_begin( p1.x, p1.y, cl );
	path_h_to( p2.x );
	path_end();
}

////////////////////////////////////////

void draw::vline( const point &p1, const point &p2, Class cl )
{
	path_begin( p1.x, p1.y, cl );
	path_v_to( p2.y );
	path_end();
}

////////////////////////////////////////

void draw::path( const point &p1, const point &p2, float r, Arc dir, Class cl )
{
	path_begin( p1.x, p1.y, cl );
	switch ( dir )
	{
		case RIGHT_UP:
		case RIGHT_DOWN:
			path_h_to( p2.x - r );
			break;

		case LEFT_UP:
		case LEFT_DOWN:
			path_h_to( p2.x + r );
			break;

		case UP_RIGHT:
		case UP_LEFT:
			path_v_to( p2.y + r );
			break;

		case DOWN_RIGHT:
		case DOWN_LEFT:
			path_v_to( p2.y - r );
			break;
	}

	path_arc( r, dir );
	switch ( dir )
	{
		case RIGHT_UP:
		case RIGHT_DOWN:
		case LEFT_UP:
		case LEFT_DOWN:
			path_v_to( p2.y );
			break;

		case UP_RIGHT:
		case UP_LEFT:
		case DOWN_RIGHT:
		case DOWN_LEFT:
			path_h_to( p2.x );
			break;
	}
	path_end();
}

////////////////////////////////////////

void draw::path( Direction d1, const point &p1, const point &p2, Direction d2, float r, Class cl )
{
	path_begin( p1.x, p1.y, cl );

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
							path_arc( r, RIGHT_DOWN );
							path_v_to( p2.y - r );
							path_arc( r, DOWN_RIGHT );
							path_h_to( p2.x );
						}
						else
						{
							path_h_to( p2.x - r*2 );
							path_arc( r, RIGHT_UP );
							path_v_to( p2.y + r );
							path_arc( r, UP_RIGHT );
						}
					}
					break;

				case LEFT:
					path_h_to( std::max( p1.x, p2.x ) );
					if ( p1.y < p2.y )
					{
						path_arc( r, RIGHT_DOWN );
						path_v_to( p2.y - r );
						path_arc( r, DOWN_LEFT );
						path_h_to( p2.x );
					}
					else
					{
						path_h_to( std::max( p1.x, p2.x ) );
						path_arc( r, RIGHT_UP );
						path_v_to( p2.y + r );
						path_arc( r, UP_LEFT );
						path_h_to( p2.x );
					}
					break;

				case UP:
					path_h_to( p2.x - r );
					path_arc( r, RIGHT_UP );
					path_v_to( p2.y );
					break;

				case DOWN:
					path_h_to( p2.x - r );
					path_arc( r, RIGHT_DOWN );
					path_v_to( p2.y );
					break;


				default:
					break;
			}
			break;

		case LEFT:
			switch ( d2 )
			{
				case RIGHT:
					path_h_to( std::min( p1.x, p2.x ) );
					if ( p1.y < p2.y )
					{
						path_arc( r, LEFT_DOWN );
						path_v_to( p2.y - r );
						path_arc( r, DOWN_RIGHT );
						path_h_to( p2.x );
					}
					else
					{
						path_arc( r, LEFT_UP );
						path_v_to( p2.y + r );
						path_arc( r, UP_RIGHT );
						path_h_to( p2.x );
					}
					break;

				case LEFT:
					if ( p1.x < p2.x )
					{
						if ( p1.y < p2.y )
						{
							path_arc( r, LEFT_DOWN );
							path_v_to( p2.y - r );
							path_arc( r, DOWN_LEFT );
							path_h_to( p2.x );
						}
						else
						{
							path_h_to( p2.x - r*2 );
							path_arc( r, LEFT_UP );
							path_v_to( p2.y + r );
							path_arc( r, UP_LEFT );
						}
					}
					else
					{
						if ( p1.y < p2.y )
						{
							path_arc( r, LEFT_DOWN );
							path_v_to( p2.y - r );
							path_arc( r, DOWN_LEFT );
							path_h_to( p2.x );
						}
						else
						{
							path_h_to( p2.x + r*2 );
							path_arc( r, LEFT_UP );
							path_v_to( p2.y + r );
							path_arc( r, UP_LEFT );
						}
					}
					break;

				case UP:
					path_h_to( p2.x - r );
					path_arc( r, LEFT_UP );
					path_v_to( p2.y );
					break;

				case DOWN:
					path_h_to( p2.x + r );
					path_arc( r, LEFT_DOWN );
					path_v_to( p2.y );
					break;
			}
			break;

		case UP:
			switch ( d2 )
			{
				case RIGHT:
					path_v_to( p2.y + r );
					path_arc( r, UP_RIGHT );
					path_h_to( p2.x );
					break;

				default:
					throw runtime_error( "Not yet implemented" );
			}
			break;

		case DOWN:
			switch ( d2 )
			{
				case RIGHT:
					path_v_to( p2.y - r );
					path_arc( r, DOWN_RIGHT );
					path_h_to( p2.x );
					break;

				case LEFT:
					path_v_to( p2.y - r );
					path_arc( r, DOWN_LEFT );
					path_h_to( p2.x );
					break;

				case UP:
					path_v_to( std::max( p1.y, p2.y ) );
					if ( p2.x < p1.x )
					{
						path_arc( r, DOWN_LEFT );
						path_h_to( p2.x+r );
						path_arc( r, LEFT_UP );
					}
					else
					{
						path_arc( r, DOWN_RIGHT );
						path_h_to( p2.x-r );
						path_arc( r, RIGHT_UP );
					}
					path_v_to( p2.y );
					break;

				case DOWN:
					path_v_to( std::max( p1.y, p2.y ) - r*2 );
					if ( p2.x < p1.x )
					{
						path_arc( r, DOWN_LEFT );
						path_h_to( p2.x+r );
						path_arc( r, LEFT_DOWN );
					}
					else
					{
						path_arc( r, DOWN_RIGHT );
						path_h_to( p2.x-r );
						path_arc( r, RIGHT_DOWN );
					}
					break;

			}
			break;

		default:
			break;
	}

	path_end();
}

////////////////////////////////////////

void draw::arrow_left( const point &p, float l, float size, Class cl1, Class cl2 )
{
	if ( l > size/2 )
	{
		path_begin( p.x, p.y, cl1 );
		out << " h " << -l + size/2;
		path_end();
	}

	path_begin( p.x - l, p.y, cl2 );
	path_arrow_left( size );
	path_end();
}

////////////////////////////////////////

void draw::arrow_right( const point &p, float l, float size, Class cl1, Class cl2 )
{
	if ( l > size/2 )
	{
		path_begin( p.x, p.y, cl1 );
		out << " h " << l - size/2;
		path_end();
	}

	path_begin( p.x + l, p.y, cl2 );
	path_arrow_right( size );
	path_end();
}

////////////////////////////////////////

void draw::arrow_down( const point &p, float l, float size, Class cl1, Class cl2 )
{
	if ( l > size/2 )
	{
		path_begin( p.x, p.y, cl1 );
		out << " v " << l - size/2;
		path_end();
	}

	path_begin( p.x, p.y + l, cl2 );
	path_arrow_down( size );
	path_end();
}

////////////////////////////////////////

