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

#include <cmath>
#include <map>
#include <vector>
#include "draw.h"

class node;

using namespace std;

struct svg_box
{
	svg_box( void )
		: _p1( 0, 0 ), _p2( 0, 0 ), _xanchor( 0 ), _yanchor( 0 )
	{
	}

	svg_box( float x, float y )
		: _p1( x, y ), _p2( x, y ), _xanchor( 0 ), _yanchor( 0 )
	{
	}

	void init( float xa, float ya )
	{
		_xanchor = xa;
		_yanchor = ya;
		_p1.x = _p1.y = 0.F;
		_p2.x = _p2.y = 0.F;
	}

	void set_y_anchor( float y )
	{
		_yanchor = y;
	}

	void include( const svg_box &o, bool keep_anchor = true )
	{
		if ( keep_anchor )
		{
			if ( o._p1.y < _p1.y )
				_yanchor += _p1.y - o._p1.y;
		}
		_p1 = _p1.min( o._p1 );
		_p2 = _p2.max( o._p2 );
	}

	void include( const point &p, bool keep_anchor = true )
	{
		if ( keep_anchor )
		{
			if ( p.y < _p1.y )
				_yanchor += _p1.y - p.y;
		}
		_p1 = _p1.min( p );
		_p2 = _p2.max( p );
	}

	void move_to( const point &p )
	{
		_p2.x += p.x - _p1.x;
		_p2.y += p.y - _p1.y;
		_p1.x += p.x;
		_p1.y += p.y;
	}

	void move_by( const point &p )
	{
		_p1.x += p.x;
		_p2.x += p.x;
		_p1.y += p.y;
		_p2.y += p.y;
	}

	void move_l_anchor( const point &p )
	{
		float w = width(), h = height();
		_p1.x = p.x;
		_p1.y = p.y - _yanchor;
		_p2.x = _p1.x + w;
		_p2.y = _p1.y + h;
	}

	void move_r_anchor( const point &p )
	{
		float w = width(), h = height();
		_p2.x = p.x;
		_p1.y = p.y - _yanchor;
		_p1.x = _p2.x - w;
		_p2.y = _p1.y + h;
	}

	point l_anchor( void ) const { return point( _p1.x, _p1.y + _yanchor ); }
	point r_anchor( void ) const { return point( _p2.x, _p1.y + _yanchor ); }
	point c_anchor( void ) const { return point( (_p1.x + _p2.x )/2, _p1.y + _yanchor ); }

	point tl_anchor( void ) const { return point( _p1.x + _xanchor, _p1.y ); }
	point bl_anchor( void ) const { return point( _p1.x + _xanchor, _p2.y ); }

	point tr_anchor( void ) const { return point( _p2.x - _xanchor, _p1.y ); }
	point br_anchor( void ) const { return point( _p2.x - _xanchor, _p2.y ); }

	point t_center( void ) const { return point( ( _p1.x + _p2.x )/2, _p1.y ); }
	point b_center( void ) const { return point( ( _p1.x + _p2.x )/2, _p2.y ); }

	point tl_corner( void ) const { return _p1; }
	point tr_corner( void ) const { return point( _p2.x, _p1.y ); }
	point bl_corner( void ) const { return point( _p1.x, _p2.y ); }
	point br_corner( void ) const { return _p2; }

	float x( void ) const { return _p1.x; }
	float y( void ) const { return _p1.y; }

	void set_width( float w ) { _p2.x = _p1.x + w; }
	void set_height( float h ) { _p2.y = _p1.y + h; }

	float width( void ) const { return _p2.x - _p1.x; }
	float height( void ) const { return _p2.y - _p1.y; }

private:
	point _p1, _p2;
	float _xanchor;
	float _yanchor;
};

struct svg_context
{
	svg_context( void )
		: dir( NONE ), use_left_rail( false ), use_right_rail( false )
	{
	}

	map<const node *,svg_box> data;

	Direction dir;

	bool use_left_rail;
	float left_rail;

	bool use_right_rail;
	float right_rail;

	float rail_top;
	float rail_bottom;

	void push_state( void ) { _stack.push_back( state( *this ) ); }
	void pop_state( void ) { _stack.back().pop( *this ); _stack.pop_back(); }

	void reverse( void )
	{
		switch ( dir )
		{
			case RIGHT: dir = LEFT; break;
			case LEFT: dir = RIGHT; break;
			case UP: dir = DOWN; break;
			case DOWN: dir = UP; break;
			case NONE: break;
		}
	}

private:

	struct state
	{
		state( const svg_context &ctxt )
			: dir( ctxt.dir ), x( ctxt.use_left_rail ), y( ctxt.use_right_rail ), l( ctxt.left_rail ), r( ctxt.right_rail ), t( ctxt.rail_top ), b( ctxt.rail_bottom )
		{
		}

		void pop( svg_context &ctxt )
		{
			ctxt.dir = dir;
			ctxt.use_left_rail = x;
			ctxt.use_right_rail = y;
			ctxt.left_rail = l;
			ctxt.right_rail = r;
			ctxt.rail_top = t;
			ctxt.rail_bottom = b;
		}

		Direction dir;
		bool x, y;
		float l, r;
		float t, b;
	};

	vector<state> _stack;
};

void svg_generate( ostream &out, const node *ebnf );

