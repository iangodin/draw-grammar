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
#include "node.h"
#include "print.h"

#define TEXT_SIZE 24.F
#define PADBOX 4.F 
#define PADV 8.F
#define PADH 12.F
#define INDENT 24.F
#define OVER_HEIGHT 24.F
#define SPLIT_WIDTH 32.F
#define RADIUS 8.F

static int uid = 0;

////////////////////////////////////////

svg_data &
compute_size( svg_context &ctxt, const node *node )
{
	svg_data &data = ctxt.data[node];
	data.width = data.height = 0;

	if ( node == NULL )
		return ctxt.data[node];

	if ( const ebnf *n = dynamic_cast<const ebnf*>( node ) )
	{
		data.add_height( compute_size( ctxt, n->prods() ) );
		data.width += INDENT;
		data.add_height( compute_size( ctxt, n->title() ) );
		data.add_height( compute_size( ctxt, n->comment() ) );
	}
	else if ( const productions *n = dynamic_cast<const productions*>( node ) )
	{
		for ( size_t i = 0; i < n->size(); ++i )
			data.add_height( compute_size( ctxt, n->at( i ) ) );

	}
	else if ( const production *n = dynamic_cast<const production*>( node ) )
	{
		data.add_width( compute_size( ctxt, n->id() ) );
		data.add_width( compute_size( ctxt, n->expr() ) );
		data.width += INDENT;
	}
	else if ( const expression *n = dynamic_cast<const expression*>( node ) )
	{
		for ( size_t i = 0; i < n->size(); ++i )
			data.add_height( compute_size( ctxt, n->at( i ) ) );
		if ( n->size() > 1 )
			data.width += SPLIT_WIDTH * 2.F;
	}
	else if ( const term *n = dynamic_cast<const term*>( node ) )
	{
		for ( size_t i = 0; i < n->size(); ++i )
			data.add_width( compute_size( ctxt, n->at( i ) ) );
	}
	else if ( const repetition *n = dynamic_cast<const repetition*>( node ) )
	{
		data.add_height( compute_size( ctxt, n->expr() ) );
		data.height += OVER_HEIGHT;
		data.width += SPLIT_WIDTH * 2.F;
	}
	else if ( const optional *n = dynamic_cast<const optional*>( node ) )
	{
		data.add_height( compute_size( ctxt, n->expr() ) );
		data.height += OVER_HEIGHT;
		data.width += SPLIT_WIDTH * 2.F;
	}
	else if ( const identifier *n = dynamic_cast<const identifier*>( node ) )
	{
		data.height = TEXT_SIZE + PADV * 2.F;
		data.width = float( n->value().size() ) * TEXT_SIZE * 0.60F + PADH * 2.F;
	}
	else if ( const literal *n = dynamic_cast<const literal*>( node ) )
	{
		data.height = TEXT_SIZE + PADV * 2.F;
		data.width = float( n->value().size() + 2 ) * TEXT_SIZE * 0.60F + PADH * 2.F;
	}
	else
		throw runtime_error( "unknown node type" );

	return data;
}

////////////////////////////////////////

svg_data &
compute_location( svg_context &ctxt, const node *node, float x, float y )
{
	if ( node == NULL )
		return ctxt.data[node];

	svg_data &data = ctxt.data[node];
	data.x = x;
	data.y = y;

	if ( const ebnf *n = dynamic_cast<const ebnf*>( node ) )
	{
		svg_data &title = compute_location( ctxt, n->title(), x, y );
		svg_data &prods = compute_location( ctxt, n->prods(), x + INDENT, y + title.height );
		compute_location( ctxt, n->comment(), x, y + title.height + prods.height );
	}
	else if ( const productions *n = dynamic_cast<const productions*>( node ) )
	{
		for ( size_t i = 0; i < n->size(); ++i )
		{
			svg_data &d = compute_location( ctxt, n->at( i ), x, y );
			y += d.height;
		}
	}
	else if ( const production *n = dynamic_cast<const production*>( node ) )
	{
		svg_data &id = compute_location( ctxt, n->id(), x, y );
		compute_location( ctxt, n->expr(), x + id.width, y );
	}
	else if ( const expression *n = dynamic_cast<const expression*>( node ) )
	{
		if ( n->size() > 1 )
			x += SPLIT_WIDTH;
		for ( size_t i = 0; i < n->size(); ++i )
		{
			svg_data &d = compute_location( ctxt, n->at( i ), x, y );
			y += d.height;
		}
	}
	else if ( const term *n = dynamic_cast<const term*>( node ) )
	{
		for ( size_t i = 0; i < n->size(); ++i )
		{
			svg_data &d = compute_location( ctxt, n->at( i ), x, y );
			x += d.width;
		}
	}
	else if ( const repetition *n = dynamic_cast<const repetition*>( node ) )
	{
		compute_location( ctxt, n->expr(), x + SPLIT_WIDTH, y + OVER_HEIGHT );
	}
	else if ( const optional *n = dynamic_cast<const optional*>( node ) )
	{
		compute_location( ctxt, n->expr(), x + SPLIT_WIDTH, y + OVER_HEIGHT );
	}
	else if ( dynamic_cast<const identifier*>( node ) )
	{
	}
	else if ( dynamic_cast<const literal*>( node ) )
	{
	}
	else
		throw runtime_error( "unknown node type" );

	return data;
}

inline void box( ostream &out, float x, float y, float w, float h, const string &color = "white" )
{
	x += PADH - PADBOX;
	y += PADV - PADBOX;
	w -= PADH * 2.F - PADBOX * 2.F;
	h -= PADV * 2.F - PADBOX * 2.F;
	out << "  <rect x=\"" << x+0.5F << "\" y=\"" << y+0.5F << "\" width=\"" << w << "\" height=\"" << h << "\" stroke=\"" << color << "\" fill=\"transparent\" stroke-width=\"1.5\"/>\n";
}

inline void round( ostream &out, float x, float y, float w, float h )
{
	x += PADH - PADBOX;
	y += PADV - PADBOX;
	w -= ( PADH - PADBOX ) * 2.F;
	h -= ( PADV - PADBOX ) * 2.F;
	out << "  <rect rx=\"" << h/2.F << "\" ry=\"" << h/2.F << "\" x=\"" << x + 0.5F << "\" y=\"" << y + 0.5F << "\" width=\"" << w << "\" height=\"" << h << "\" stroke=\"white\" fill=\"transparent\" stroke-width=\"1.5\"/>\n";
}

inline void text( ostream &out, float x, float y, float w, float h, const string &text, const string &color = "white" )
{
	out << "  <text x=\"" << x + w/2.F << "\" y=\"" << y + h/2.F + TEXT_SIZE/2.F - TEXT_SIZE/8.F << "\" fill=\"" << color << "\" text-anchor=\"middle\">";
	out << text << "</text>\n";
}

inline void path_begin( ostream &out, float x, float y )
{
	out << "  <path d=\"M " << x << ' ' << y;
}

inline void path_horizontal_to( ostream &out, float x )
{
	out << " H " << x;
}

inline void path_vertical_to( ostream &out, float y )
{
	out << " V " << y;
}

inline void path_line_to( ostream &out, float x, float y )
{
	out << " L " << x << ' ' << y;
}

inline void path_loop_in( ostream &out, float x, float y1, float y2 )
{
	float r = RADIUS;
	float d = SPLIT_WIDTH/2.F - RADIUS;
	out << " M " << x - d << ' ' << y1;
	out << " A " << r << ' ' << r << " 0 0 0 " << x - d - r << ' ' << y1 + r;
	out << " V " << y2 - r;
	out << " A " << r << ' ' << r << " 0 0 0 " << x - d << ' ' << y2;
	out << " H " << x;
}

inline void path_loop_out( ostream &out, float x, float y1, float y2 )
{
	float r = RADIUS;
	float d = SPLIT_WIDTH/2.F - RADIUS;
	out << " M " << x + d << ' ' << y1;
	out << " A " << r << ' ' << r << " 0 0 1 " << x + d + r << ' ' << y1 + r;
	out << " V " << y2 - r;
	out << " A " << r << ' ' << r << " 0 0 1 " << x + d << ' ' << y2;
	out << " H " << x;
}

inline void path_split( ostream &out, float x1, float y1, float x2, float y2 )
{
	if ( y1 == y2 )
		path_horizontal_to( out, x2 );
	else
	{
		float r = RADIUS;
		x1 += SPLIT_WIDTH/2.F - RADIUS;
		out << " M " << x1 << ' ' << y1;
		out << " A " << r << ' ' << r << " 0 0 1 " << x1 + r << ' ' << y1 + r;
		out << " V " << y2 - r;
		out << " A " << r << ' ' << r << " 0 0 0 " << x1 + r * 2.F << ' ' << y2;
		out << " H " << x2;
	}
}

inline void path_join( ostream &out, float x1, float y1, float x2, float y2 )
{
	if ( y1 == y2 )
		path_horizontal_to( out, x2 );
	else
	{
		float r = RADIUS;
		float d = SPLIT_WIDTH/2.F - RADIUS;
		x2 -= d;
		path_horizontal_to( out, x2 - r * 2.F );
		out << " A " << r << ' ' << r << " 0 0 0 " << x2 - r << ' ' << y1 - r;
		out << " V " << y2 + r;
		out << " A " << r << ' ' << r << " 0 0 1 " << x2 << ' ' << y2;
	}
}

inline void path_arrow_left( ostream &out )
{
	out << " m 1 0 l 6 -3 0 6 z";
}

inline void path_arrow_right( ostream &out )
{
	out << " l -6 -3 0 6 z";
}

inline void path_end( ostream &out )
{
	out <<  "\" stroke-width=\"1.5\" stroke=\"white\" fill=\"transparent\"/>\n";
}

inline void path_end_fill( ostream &out )
{
	out <<  "\" stroke-width=\"1.5\" stroke=\"white\" fill=\"white\"/>\n";
}

////////////////////////////////////////

void draw_svg( ostream &out, const node *node, svg_context &ctxt )
{
	svg_data &data = ctxt.data[node];

	if ( const ebnf *n = dynamic_cast<const ebnf*>( node ) )
	{
		draw_svg( out, n->title(), ctxt );
		draw_svg( out, n->prods(), ctxt );
		draw_svg( out, n->comment(), ctxt );
	}
	else if ( const productions *n = dynamic_cast<const productions*>( node ) )
	{
		for ( size_t i = 0; i < n->size(); ++i )
			draw_svg( out, n->at( i ), ctxt );
	}
	else if ( const production *n = dynamic_cast<const production*>( node ) )
	{
		svg_data &i = ctxt.data[n->id()];
		svg_data &e = ctxt.data[n->expr()];

		path_begin( out, i.x + i.width - PADH + PADBOX, i.y + TEXT_SIZE/2.F + PADV );
		path_horizontal_to( out, e.x + PADH - PADBOX );
		path_end( out );

		svg_data &d = ctxt.data[n];
		float x = d.x + d.width - PADH + PADBOX;
		float y = d.y + TEXT_SIZE/2.F + PADV;
		path_begin( out, x, y );
		path_horizontal_to( out, x - INDENT );
		path_end( out );

		path_begin( out, x, y );
		path_arrow_right( out );
		path_end_fill( out );

		draw_svg( out, n->id(), ctxt );
		draw_svg( out, n->expr(), ctxt );
	}
	else if ( const expression *n = dynamic_cast<const expression*>( node ) )
	{
		svg_data &d = ctxt.data[n];
		float msx = d.x + PADH - PADBOX;
		float msy = d.y + TEXT_SIZE/2.F + PADV;
		float mex = d.x + d.width - PADH + PADBOX;
		float mey = msy;
		for ( size_t i = 0; i < n->size(); ++i )
		{
			svg_data &e = ctxt.data[n->at( i )];
			float sx = e.x + PADH - PADBOX;
			float sy = e.y + TEXT_SIZE/2.F + PADV;
			float ex = e.x + e.width - PADH + PADBOX;
			float ey = sy;
			path_begin( out, msx, msy );
			path_split( out, msx, msy, sx, sy );
			path_end( out );

			path_begin( out, sx, sy );
			path_arrow_right( out );
			path_end_fill( out );

			path_begin( out, ex, ey );
			path_join( out, ex, ey, mex, mey );
			path_end( out );
			if ( mex - ex > TEXT_SIZE * 3 )
			{
				path_begin( out, ( mex + ex ) / 2.F, ey );
				path_arrow_right( out );
				path_end_fill( out );
			}

			draw_svg( out, n->at( i ), ctxt );
		}
	}
	else if ( const term *n = dynamic_cast<const term*>( node ) )
	{
		svg_data &d = ctxt.data[n];
		float sx = d.x + PADH - PADBOX;
		float y = d.y + TEXT_SIZE/2.F + PADV;
		for ( size_t i = 0; i < n->size(); ++i )
		{
			svg_data &e = ctxt.data[n->at( i )];
			if ( i > 0 )
			{
				float ex = e.x + PADH - PADBOX;
				path_begin( out, sx, y );
				path_horizontal_to( out, ex );
				path_end( out );

				path_begin( out, ex, y );
				path_arrow_right( out );
				path_end_fill( out );
			}
			sx = e.x + e.width - PADH + PADBOX;
			draw_svg( out, n->at( i ), ctxt );
		}
	}
	else if ( const repetition *n = dynamic_cast<const repetition*>( node ) )
	{
		svg_data &d = ctxt.data[n];
		float sx = d.x + PADH - PADBOX;
		float sy = d.y + TEXT_SIZE/2.F + PADV;
		float ex = d.x + d.width - PADH + PADBOX;
		path_begin( out, sx, sy );
		path_horizontal_to( out, d.x + d.width - PADH + PADBOX );
		path_end( out );

		path_begin( out, ( sx + ex ) / 2.F, sy );
		path_arrow_right( out );
		path_end_fill( out );

		svg_data &e = ctxt.data[n->expr()];
		ex = e.x + PADH - PADBOX;
		float ey = e.y + TEXT_SIZE/2.F + PADV;
		path_begin( out, ex, sy );
		path_loop_in( out, ex, sy, ey );
		path_end( out );

		ex = e.x + e.width - PADH + PADBOX;
		path_begin( out, ex, sy );
		path_loop_out( out, ex, sy, ey );
		path_end( out );

		path_begin( out, ex, ey );
		path_arrow_left( out );
		path_end_fill( out );

		draw_svg( out, n->expr(), ctxt );
	}
	else if ( const optional *n = dynamic_cast<const optional*>( node ) )
	{
//		box( out, data.x, data.y, data.width, data.height, "magenta" );
		svg_data &d = ctxt.data[n];
		float sx = d.x + PADH - PADBOX;
		float sy = d.y + TEXT_SIZE/2.F + PADV;
		float ex = d.x + d.width - PADH + PADBOX;

		path_begin( out, sx, sy );
		path_horizontal_to( out, d.x + d.width - PADH + PADBOX );
		path_end( out );

		path_begin( out, ( sx + ex ) / 2.F, sy );
		path_arrow_right( out );
		path_end_fill( out );

		svg_data &e = ctxt.data[n->expr()];
		ex = e.x + PADH - PADBOX;
		float ey = e.y + TEXT_SIZE/2.F + PADV;

		path_begin( out, sx, sy );
		path_split( out, sx, sy, ex, ey );
		path_end( out );

		path_begin( out, ex, ey );
		path_arrow_right( out );
		path_end_fill( out );

		sx = d.x + d.width - PADH + PADBOX;
		ex = e.x + e.width - PADH + PADBOX;
		path_begin( out, ex, ey );
		path_join( out, ex, ey, sx, sy );
		path_end( out );

		draw_svg( out, n->expr(), ctxt );
	}
	else if ( const literal *n = dynamic_cast<const literal*>( node ) )
	{
		round( out, data.x, data.y, data.width, data.height );
		text( out, data.x, data.y, data.width, data.height, n->value(), "blue" );
	}
	else if ( const identifier *n = dynamic_cast<const identifier*>( node ) )
	{
		box( out, data.x, data.y, data.width, data.height );
		text( out, data.x, data.y, data.width, data.height, n->value(), "red" );
	}
}

////////////////////////////////////////

void svg_generate( ostream &out, const node *e )
{
	const ebnf *n = dynamic_cast<const ebnf*>( e );
	if ( !n )
		throw runtime_error( "invalid ebnf node" );

	svg_context ctxt;
	compute_size( ctxt, n );
	compute_location( ctxt, n, 0, 0 );

	svg_data &top = ctxt.data[n];
	out <<
		"<html><head>"
		"  <title>Grammar</title>"
		"  <meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\"></meta>"
		"  <link type=\"text/css\" rel=\"stylesheet\" href=\"style.css\"></link>"
		"</head>\n";

	out << "<body><svg xmlns:svg=\"http://www.w3.org/2000/svg\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" "
	   " width=\"" << top.width << "\" height=\"" << top.height << "\">\n";
	draw_svg( out, n, ctxt );
	out << "</body></html>\n";
}

////////////////////////////////////////

