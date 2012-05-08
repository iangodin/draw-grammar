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

#pragma once

#include <iostream>
#include <string>

using namespace std;

enum Direction
{
	NONE,
	UP,
	DOWN,
	LEFT,
	RIGHT
};

enum Arc
{
	RIGHT_UP,
	RIGHT_DOWN,
	LEFT_UP,
	LEFT_DOWN,
	UP_RIGHT,
	UP_LEFT,
	DOWN_RIGHT,
	DOWN_LEFT
};

struct point
{
	point( void )
		: x( 0 ), y( 0 )
	{
	}

	point( float a, float b )
		: x( a ), y( b )
	{
	}

	point( const point &p )
		: x( p.x ), y( p.y )
	{
	}

	inline point move( float dx, float dy ) const
	{
		return point( x + dx, y + dy );
	}

	inline point move( const point &d ) const
	{
		return point( x + d.x, y + d.y );
	}

	inline point min( const point &p ) const
	{
		return point( std::min( x, p.x ), std::min( y, p.y ) );
	}

	inline point max( const point &p ) const
	{
		return point( std::max( x, p.x ), std::max( y, p.y ) );
	}

	inline point negate( void ) const
	{
		return point( -x, -y );
	}

	point &operator=( const point &p )
	{
		x = p.x;
		y = p.y;
		return *this;
	}

	float x, y;
};

void push_translate( const point &p );
void pop_translate( void );

void id_begin( ostream &out, float x, float y, float w, float h, const string &name );
void id_end( ostream &out );

void link_begin( ostream &out, const string &name );
void link_end( ostream &out );

void circle( ostream &out, float x, float y, float r, const string &cl );
void box( ostream &out, float x, float y, float w, float h, const string &cl );
void round( ostream &out, float x, float y, float w, float h, const string &c );
void text( ostream &out, float x, float y, float w, float h, const string &text, const string &cl );
void text_center( ostream &out, float x, float y, float w, float h, const string &text, const string &cl );
void hline( ostream &out, const point &p1, const point &p2, const string &cl );
void vline( ostream &out, const point &p1, const point &p2, const string &cl );
void path( ostream &out, point &p1, point &p2, float r, Arc dir, const string &cl );
void path( ostream &out, Direction d1, const point &p1, const point &p2, Direction d2, float r, const string &cl );

void arrow_left( ostream &out, const point &p, float l, float size, const string &cl1, const string &cl2 );
void arrow_right( ostream &out, const point &p, float l, float size, const string &cl1, const string &cl2 );
void arrow_down( ostream &out, const point &p, float l, float size, const string &cl1, const string &cl2 );

void path_begin( ostream &out, float x, float y );

void path_h_to( ostream &out, float x );
void path_v_to( ostream &out, float y );
void path_line_to( ostream &out, float x, float y );
void path_arc( ostream &out, float r, Arc a );
void path_arrow_left( ostream &out, float size );
void path_arrow_right( ostream &out, float size );
void path_arrow_down( ostream &out, float size );

void path_end( ostream &out, const string &cl );

