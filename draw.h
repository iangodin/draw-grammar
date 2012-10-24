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
#include <vector>

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

enum Class
{
	LINE,
	ARROW,
	BOX,
	PRODUCTION,
	LITERAL,
	IDENTIFIER,
	OTHER,
	END
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

class draw
{
public:
	draw( ostream &out );
	virtual ~draw( void );

	virtual void push_translate( const point &p );
	virtual void pop_translate( void );

	virtual void id_begin( float x, float y, float w, float h, const string &name ) = 0;
	virtual void id_end() = 0;

	virtual void link_begin( const string &name ) = 0;
	virtual void link_end() = 0;

	virtual void circle( float x, float y, float r, Class cl ) = 0;
	virtual void box( float x, float y, float w, float h, Class cl ) = 0;
	virtual void round( float x, float y, float w, float h, Class c ) = 0;

	virtual void text( float x, float y, float w, float h, const string &text, Class cl ) = 0;
	virtual void text_center( float x, float y, float w, float h, const string &text, Class cl ) = 0;

	virtual void hline( const point &p1, const point &p2, Class cl );
	virtual void vline( const point &p1, const point &p2, Class cl );

	virtual void path( const point &p1, const point &p2, float r, Arc dir, Class cl );
	virtual void path( Direction d1, const point &p1, const point &p2, Direction d2, float r, Class cl );

	virtual void arrow_left( const point &p, float l, float size, Class cl1, Class cl2 );
	virtual void arrow_right( const point &p, float l, float size, Class cl1, Class cl2 );
	virtual void arrow_down( const point &p, float l, float size, Class cl1, Class cl2 );

	virtual void path_begin( float x, float y, Class cl ) = 0;

	virtual void path_h_by( float x ) = 0;
	virtual void path_v_by( float y ) = 0;
	virtual void path_h_to( float x ) = 0;
	virtual void path_v_to( float y ) = 0;
	virtual void path_to( float x, float y ) = 0;
	virtual void path_arc( float r, Arc a ) = 0;
	virtual void path_arrow_left( float size ) = 0;
	virtual void path_arrow_right( float size ) = 0;
	virtual void path_arrow_down( float size ) = 0;

	virtual void path_end( void ) = 0;

protected:
	ostream &out;

	inline float xx( float x ) { return x - dx.back(); }
	inline float yy( float y ) { return y - dy.back(); }

	vector<float> dx;
	vector<float> dy;
};

