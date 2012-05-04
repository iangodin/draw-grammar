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

class node;

using namespace std;

struct svg_data
{
	svg_data( void )
		: x( 0 ), y( 0 ), width( 0 ), height( 0 )
	{
	}

	void indent( void )
	{
		width += 2.F;
	}

	void add_height( const svg_data &o )
	{
		width = max( width, o.width );
		height += o.height;
	}

	void add_width( const svg_data &o )
	{
		width += o.width;
		height = max( height, o.height );
	}

	float x, y;
	float width;
	float height;
};

struct svg_context
{
	map<const node *,svg_data> data;
};

void svg_generate( ostream &out, const node *ebnf );

