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
#define SPLIT_WIDTH 24.F
#define OVER_HEIGHT 24.F

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
		data.add_height( compute_size( ctxt, n->title() ) );
		data.add_height( compute_size( ctxt, n->comment() ) );
	}
	else if ( const productions *n = dynamic_cast<const productions*>( node ) )
	{
		data.add_height( compute_size( ctxt, n->prev() ) );
		data.add_height( compute_size( ctxt, n->value() ) );
		data.width += INDENT;
		data.height += PADV;
	}
	else if ( const production *n = dynamic_cast<const production*>( node ) )
	{
		data.add_width( compute_size( ctxt, n->id() ) );
		data.add_width( compute_size( ctxt, n->expr() ) );
	}
	else if ( const expression *n = dynamic_cast<const expression*>( node ) )
	{
		data.add_height( compute_size( ctxt, n->prev() ) );
		data.add_height( compute_size( ctxt, n->value() ) );
		if( n->prev() )
			data.width += SPLIT_WIDTH * 2.F;
	}
	else if ( const term *n = dynamic_cast<const term*>( node ) )
	{
		data.add_width( compute_size( ctxt, n->prev() ) );
		data.add_width( compute_size( ctxt, n->value() ) );
	}
	else if ( const group *n = dynamic_cast<const group*>( node ) )
	{
		data.add_width( compute_size( ctxt, n->expr() ) );
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
		data.width = float( n->value().size() ) * TEXT_SIZE + PADH * 2.F;
	}
	else if ( const literal *n = dynamic_cast<const literal*>( node ) )
	{
		data.height = TEXT_SIZE + PADV * 2.F;
		data.width = float( n->value().size() + 2 ) * TEXT_SIZE + PADH * 2.F;
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
		svg_data &prev = compute_location( ctxt, n->prev(), x, y );
		compute_location( ctxt, n->value(), x, y + prev.height + PADV );
	}
	else if ( const production *n = dynamic_cast<const production*>( node ) )
	{
		svg_data &id = compute_location( ctxt, n->id(), x, y );
		compute_location( ctxt, n->expr(), x + id.width, y );
	}
	else if ( const expression *n = dynamic_cast<const expression*>( node ) )
	{
		svg_data &prev = compute_location( ctxt, n->prev(), x, y );
		compute_location( ctxt, n->value(), x, y + prev.height );
	}
	else if ( const term *n = dynamic_cast<const term*>( node ) )
	{
		svg_data &prev = compute_location( ctxt, n->prev(), x, y );
		compute_location( ctxt, n->value(), x + prev.width, y );
	}
	else if ( const group *n = dynamic_cast<const group*>( node ) )
	{
		compute_location( ctxt, n->expr(), x, y );
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

////////////////////////////////////////

inline void line( ostream &out, float x, float y, float w, float h )
{
	out << "  <line x1=\"" << x+0.5F << "\" y1=\"" << y+0.5F << "\" x2=\"" << x + w+0.5F << "\" y2=\"" << y + h+0.5F << "\" stroke=\"white\" stroke-width=\"1.5\"/>\n";
}

inline void box( ostream &out, float x, float y, float w, float h, const string &color = "white" )
{
	x += PADH - PADBOX;
	y += PADV - PADBOX;
	w -= PADH * 2.F - PADBOX * 2.F;
	h -= PADV * 2.F - PADBOX * 2.F;
	out << "  <rect x=\"" << x+0.5F << "\" y=\"" << y+0.5F << "\" width=\"" << w << "\" height=\"" << h << "\" stroke=\"" << color << "\" fill=\"black\" stroke-width=\"1.5\"/>\n";
}

inline void round( ostream &out, float x, float y, float w, float h )
{
	x += PADH - PADBOX;
	y += PADV - PADBOX;
	w -= ( PADH - PADBOX ) * 2.F;
	h -= ( PADV - PADBOX ) * 2.F;
	out << "  <rect rx=\"" << h/2.F << "\" ry=\"" << h/2.F << "\" x=\"" << x + 0.5F << "\" y=\"" << y + 0.5F << "\" width=\"" << w << "\" height=\"" << h << "\" stroke=\"white\" fill=\"black\" stroke-width=\"1.5\"/>\n";
}

inline void loop( ostream &out, float x, float y, float w, float h )
{
	out << "  <rect rx=\"" << TEXT_SIZE/2.F << "\" ry=\"" << TEXT_SIZE/2.F << "\" x=\"" << x+0.5F << "\" y=\"" << y+0.5F << "\" width=\"" << w << "\" height=\"" << h << "\" stroke=\"white\" fill=\"transparent\" stroke-width=\"1.5\"/>\n";
}

inline void text( ostream &out, float x, float y, float w, float h, const string &text, const string &color = "white" )
{
	out << "  <text x=\"" << x + w/2.F << "\" y=\"" << y + h - PADV - TEXT_SIZE/8.F << "\" fill=\"" << color << "\" text-anchor=\"middle\">";
	out << text << "</text>\n";
}

////////////////////////////////////////

void svg_line( ostream &out, const node *node, svg_data &data, svg_context &ctxt )
{
	//box( out, data.x, data.y, data.width, data.height, "magenta" );
	if ( const term *n = dynamic_cast<const term*>( node ) )
	{
		svg_data &prev = ctxt.data[n->prev()];
		svg_data &val = ctxt.data[n->value()];
		line( out, prev.x + PADH, data.y + TEXT_SIZE/2.F + PADV, val.x + val.width - PADH, 0.F );
	}
	else if ( const optional *n = dynamic_cast<const optional*>( node ) )
	{
		svg_data &expr = ctxt.data[n->expr()];
	}
	else if ( const repetition *n = dynamic_cast<const repetition*>( node ) )
	{
		svg_data &expr = ctxt.data[n->expr()];
		loop( out, expr.x - SPLIT_WIDTH/2.F, data.y + TEXT_SIZE/2.F + PADV, expr.width + SPLIT_WIDTH, data.height - expr.height/2.F - TEXT_SIZE/2.F - PADV );
	}
}

////////////////////////////////////////

void svg_text( ostream &out, const node *node, svg_data &data, svg_context &ctxt )
{
	if ( const identifier *n = dynamic_cast<const identifier*>( node ) )
	{
		round( out, data.x, data.y, data.width, data.height );
		text( out, data.x + 1.F, data.y + 0.5F, data.width - 2.F, data.height - 1.F, n->value(), "red" );
	}
	else if ( const literal *n = dynamic_cast<const literal*>( node ) )
	{
		box( out, data.x, data.y, data.width, data.height );
		text( out, data.x + 1.F, data.y + 0.5F, data.width - 2.F, data.height - 1.F, n->value(), "blue" );
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
	for ( auto i: ctxt.data )
		svg_line( out, i.first, i.second, ctxt );
	for ( auto i: ctxt.data )
		svg_text( out, i.first, i.second, ctxt );
	out << "</body></html>\n";
}

////////////////////////////////////////

