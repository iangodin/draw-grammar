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

#include "draw.h"

using namespace std;

class draw_svg : public draw
{
public:
	draw_svg( ostream &o );
	virtual ~draw_svg( void );

	virtual void id_begin( float x, float y, float w, float h, const string &name );
	virtual void id_end();

	virtual void link_begin( const string &name );
	virtual void link_end();

	virtual void circle( float x, float y, float r, Class cl );
	virtual void box( float x, float y, float w, float h, Class cl );
	virtual void round( float x, float y, float w, float h, Class c );
	virtual void text( float x, float y, float w, float h, const string &text, Class cl );
	virtual void text_center( float x, float y, float w, float h, const string &text, Class cl );

	virtual void path_begin( float x, float y, Class cl );

	virtual void path_h_to( float x );
	virtual void path_v_to( float y );
	virtual void path_line_to( float x, float y );
	virtual void path_arc( float r, Arc a );
	virtual void path_arrow_left( float size );
	virtual void path_arrow_right( float size );
	virtual void path_arrow_down( float size );

	virtual void path_end( void );

protected:
	string escape( const string &t );
	string clname( Class cl );
};

