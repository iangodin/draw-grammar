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
#include "draw.h"

#define TEXT_SIZE 24.F
#define TEXT_PAD 8.F
#define LINE_HEIGHT (TEXT_SIZE+TEXT_PAD)
#define CIRCLE 4.F
#define PADV 20.F
#define PADH 20.F
#define RADIUS 8.F
#define OVER_HEIGHT (RADIUS*2+PADH*2)
#define SPLIT_WIDTH (RADIUS*2+PADH)
#define ARROW 8.F
#define SPACING 0.F
#define TEXT_RATIO 0.45F

////////////////////////////////////////

svg_box &
compute_size( svg_context &ctxt, const node *node, bool &above )
{
	svg_box &self = ctxt.data[node];
	self.init( PADH, LINE_HEIGHT/2 + PADV );
	if ( node == NULL )
		return self;

	if ( const ebnf *n = dynamic_cast<const ebnf*>( node ) )
	{
		ctxt.dir = NONE;
		above = false;
		svg_box &title = compute_size( ctxt, n->title(), above );
		svg_box &prods = compute_size( ctxt, n->prods(), above );
		svg_box &comment = compute_size( ctxt, n->comment(), above );

		prods.move_to( title.bl_corner() );
		comment.move_to( prods.bl_corner() );

		self.include( title );
		self.include( prods );
		self.include( comment );
	}
	else if ( const productions *n = dynamic_cast<const productions*>( node ) )
	{
		ctxt.dir = NONE;
		above = false;
		for ( size_t i = 0; i < n->size(); ++i )
		{
			ctxt.push_state();
			svg_box &e = compute_size( ctxt, n->at( i ), above );
			e.move_to( self.bl_corner() );
			self.include( e );
			ctxt.pop_state();
		}

//		point tl = self.tl_corner().negate();
//		for ( size_t i = 0; i < n->size(); ++i )
//		{
//			svg_box &e = ctxt.data[n->at( i )];
//			e.move_by( tl );
//		}
//		self.move_by( tl );
	}
	else if ( const production *n = dynamic_cast<const production*>( node ) )
	{
		ctxt.push_state();
		above = false;

		svg_box &id = compute_size( ctxt, n->id(), above );
		id.set_y_anchor( LINE_HEIGHT + PADH );

		ctxt.dir = RIGHT;
		svg_box &expr = compute_size( ctxt, n->expr(), above );

		expr.move_l_anchor( id.r_anchor() );
		self.include( id );
		self.include( expr );
		self.include( expr.br_corner().move( PADH*3 + CIRCLE, 0 ) );

		point tl = self.tl_corner().negate();
		id.move_by( tl );
		expr.move_by( tl );
		self.move_by( tl );
		ctxt.pop_state();
	}
	else if ( const expression *n = dynamic_cast<const expression*>( node ) )
	{
		if ( n->is_short() )
		{
			for ( size_t i = 0; i < n->size(); ++i )
			{
				ctxt.push_state();
				ctxt.use_left_rail = false;
				ctxt.use_right_rail = false;
				ctxt.dir = DOWN;
				bool new_above = false;
				svg_box &e = compute_size( ctxt, n->at( i ), new_above );
				e.move_to( self.tr_corner() );
				self.include( e );
				ctxt.pop_state();
			}
			self.set_y_anchor( - std::max( PADV, RADIUS ) );
			self.include( point( 0, -std::max( PADV, RADIUS )*2 ) );
			if ( ctxt.dir == RIGHT )
				self.include( self.br_corner().move( RADIUS*2, std::max( PADV, RADIUS )*2 ) );
			else
				self.include( self.bl_corner().move( -RADIUS*2, std::max( PADV, RADIUS )*2 ) );
		}
		else
		{
			bool new_above = false;
			ctxt.push_state();
			{
				for ( size_t i = 0; i < n->size(); ++i )
				{
					ctxt.push_state();
					ctxt.use_left_rail = true;
					ctxt.use_right_rail = true;
					svg_box &e = compute_size( ctxt, n->at( i ), new_above );
					if ( i == 0 )
						e.move_l_anchor( self.l_anchor() );
					else if ( i == 1 && !above )
					{
						point p1 = self.bl_corner();
						point p2 = self.l_anchor().move( 0, RADIUS*2 - e.l_anchor().y );
						e.move_to( p1.max( p2 ) );
					}
					else
						e.move_to( self.bl_corner() );
					self.include( e );
					ctxt.pop_state();
				}

				if ( above )
				{
					self.include( point( -RADIUS-PADH, self.l_anchor().y - RADIUS ) );
					self.include( self.br_corner().move( point( RADIUS+PADH, 0 ) ) );
				}
				else
				{
					if ( !ctxt.use_left_rail )
						self.include( point( -RADIUS*2, 0 ) );
					if ( !ctxt.use_right_rail )
						self.include( self.br_corner().move( point( RADIUS*2, 0 ) ) );
				}
			}
			ctxt.pop_state();
		}

		point tl = self.tl_corner().negate();
		for ( size_t i = 0; i < n->size(); ++i )
		{
			svg_box &e = ctxt.data[n->at(i)];
			e.move_by( tl );
		}
		self.move_by( tl );
	}
	else if ( const term *n = dynamic_cast<const term*>( node ) )
	{
		above = false;
		point anchor;
		if ( ctxt.dir == RIGHT )
			anchor = self.l_anchor();
		else
			anchor = self.r_anchor();

		for ( size_t i = 0; i < n->size(); ++i )
		{
			ctxt.push_state();
			if ( i > 0 )
				ctxt.use_left_rail = false;
			if ( i+1 < n->size() )
				ctxt.use_right_rail = false;
			svg_box &e = compute_size( ctxt, n->at( i ), above );
			if ( ctxt.dir == RIGHT )
			{
				e.move_l_anchor( anchor );
				anchor = e.r_anchor();
			}
			else
			{
				e.move_r_anchor( anchor );
				anchor = e.l_anchor();
			}
			self.include( e );
			ctxt.pop_state();
		}

		point tl = self.tl_corner().negate();
		for ( size_t i = 0; i < n->size(); ++i )
		{
			svg_box &e = ctxt.data[n->at( i )];
			e.move_by( tl );
		}
		self.move_by( tl );

		svg_box &e = ctxt.data[n->at( 0 )];
		self.set_y_anchor( e.l_anchor().y );
	}
	else if ( const repetition *n = dynamic_cast<const repetition*>( node ) )
	{
		ctxt.push_state();
		{
			ctxt.use_left_rail = false;
			ctxt.use_right_rail = false;
			ctxt.reverse();

			above = true;
			svg_box &e = compute_size( ctxt, n->expr(), above );

			self.set_y_anchor( PADV + ARROW/2 );
			if ( above )
				self.include( point( RADIUS - PADH, PADV*2 + ARROW ) );
			else
				self.include( point( RADIUS + PADH, PADV*2 + ARROW ) );

			e.move_to( self.br_corner() );

			if ( above )
			{
				e.move_by( point( 0, std::max( 0.F, RADIUS - PADV - ARROW/2 ) ) );
				self.include( e.br_corner().move( RADIUS - PADH, 0 ) );
			}
			else
			{
				float delta = RADIUS*2 - ( e.l_anchor().y - self.l_anchor().y );
				if ( delta > 0.F )
					e.move_by( point( 0, delta ) );
				self.include( e.br_corner().move( RADIUS + PADH, 0 ) );
			}

			point tl = self.tl_corner().negate();
			e.move_by( tl );
			self.move_by( tl );
		}
		above = false;
		ctxt.pop_state();
	}
	else if ( const onemore *n = dynamic_cast<const onemore*>( node ) )
	{
		above = false;
		ctxt.push_state();
		{
			ctxt.use_left_rail = false;
			ctxt.use_right_rail = false;

			svg_box &e = compute_size( ctxt, n->expr(), above );
			e.move_l_anchor( point( RADIUS + PADH, self.l_anchor().y )  );

			point tl;

			if ( n->sep() )
			{
				ctxt.reverse();
				svg_box &s = compute_size( ctxt, n->sep(), above );
				s.move_to( e.bl_corner() );
				float delta = std::max( 0.F, RADIUS*2 - ( s.r_anchor().y - e.r_anchor().y ) );
				s.move_by( point( 0, delta ) );
				self.include( e );
				self.include( s );
				self.include( e.br_corner().max( s.br_corner() ).move( RADIUS + PADH, 0 ) );

			   	tl = self.tl_corner().negate();
				s.move_by( tl );
			}
			else
			{
				point p1( e.r_anchor().move( 0, -( RADIUS*2 + ARROW/2 + PADH ) ) );
				point p2( e.tr_corner().move( 0, -( ARROW + PADH ) ) );
				self.include( e );
				self.include( p1.min( p2 ).move( RADIUS + PADH, 0 ) );
				tl = self.tl_corner().negate();
			}

			e.move_by( tl );
			self.move_by( tl );
		}
		ctxt.pop_state();
	}
	else if ( const optional *n = dynamic_cast<const optional*>( node ) )
	{
		ctxt.push_state();
		{
			ctxt.use_left_rail = false;
			ctxt.use_right_rail = false;

			above = true;
			svg_box &e = compute_size( ctxt, n->expr(), above );
			ctxt.pop_state();

			if ( above )
				self.include( point( RADIUS - PADV, std::max( PADV + ARROW + PADV, PADV + ARROW/2 + RADIUS ) ) );
			else
			{
				if ( ctxt.use_left_rail )
					self.include( point( 0, PADV*2 + ARROW ) );
				else
					self.include( point( RADIUS*2, PADV*2 + ARROW ) );
			}
			self.set_y_anchor( PADV + ARROW/2 );

			e.move_to( self.br_corner() );
			if ( above )
				self.include( e.br_corner().move( RADIUS - PADH, 0 ) );
			else
			{
				float delta = RADIUS*2 - ( e.l_anchor().y - self.l_anchor().y );
				if ( delta > 0.F )
					e.move_by( point( 0, delta ) );
				if ( ctxt.use_right_rail )
					self.include( e.br_corner() );
				else
					self.include( e.br_corner().move( RADIUS*2, 0 ) );
			}

			point tl = self.tl_corner().negate();
			e.move_by( tl );
			self.move_by( tl );
		}
		above = false;
	}
	else if ( const identifier *n = dynamic_cast<const identifier*>( node ) )
	{
		switch( ctxt.dir )
		{
			case NONE:
				self.set_width( float( n->value().size() ) * LINE_HEIGHT * TEXT_RATIO + PADH );
				self.set_height( LINE_HEIGHT + PADV * 2.F );
				break;

			case RIGHT:
			case LEFT:
				self.set_width( float( n->value().size() + 1 ) * LINE_HEIGHT * TEXT_RATIO + ARROW + PADH * 2.F );
				self.set_height( LINE_HEIGHT + PADV * 2.F );
				break;

			case UP:
			case DOWN:
				self.set_width( float( n->value().size() + 1 ) * LINE_HEIGHT * TEXT_RATIO + PADH * 2.F );
				self.set_height( LINE_HEIGHT + ARROW + PADV * 2.F );
				break;
		}
		above = false;
	}
	else if ( const literal *n = dynamic_cast<const literal*>( node ) )
	{
		switch ( ctxt.dir )
		{
			case NONE:
				self.set_width( float( n->value().size() ) * LINE_HEIGHT * TEXT_RATIO );
				self.set_height( LINE_HEIGHT + PADV * 2.F );
				break;

			case RIGHT:
			case LEFT:
				self.set_width( float( n->value().size() + 3 ) * LINE_HEIGHT * TEXT_RATIO + ARROW + PADH * 2.F );
				self.set_height( LINE_HEIGHT + PADV * 2.F );
				break;

			case UP:
			case DOWN:
				self.set_width( float( n->value().size() + 3 ) * LINE_HEIGHT * TEXT_RATIO + PADH * 2.F );
				self.set_height( LINE_HEIGHT + ARROW + PADV * 2.F );
				break;
		}
		above = false;
	}
	else if ( const other *n = dynamic_cast<const other*>( node ) )
	{
		switch ( ctxt.dir )
		{
			case NONE:
				self.set_width( float( n->value().size() + 1 ) * LINE_HEIGHT * TEXT_RATIO + PADH * 2.F );
				self.set_height( LINE_HEIGHT + PADV * 2.F );
				break;

			case RIGHT:
			case LEFT:
				self.set_width( float( n->value().size() + 1 ) * LINE_HEIGHT * TEXT_RATIO + ARROW + PADH * 2.F );
				self.set_height( LINE_HEIGHT + PADV * 2.F );
				break;

			case UP:
			case DOWN:
				self.set_width( float( n->value().size() + 1 ) * LINE_HEIGHT * TEXT_RATIO + PADH * 2.F );
				self.set_height( LINE_HEIGHT + ARROW + PADV * 2.F );
				break;
		}
		above = false;
	}
	else
		throw runtime_error( "unknown node type" );

	return self;
}

////////////////////////////////////////

void draw_svg( ostream &out, const node *node, svg_context &ctxt, bool &above )
{
	svg_box &self = ctxt.data[node];
	ctxt.push_state();

	ctxt.left_rail += self.x();
	ctxt.right_rail += self.x();
	ctxt.rail_top += self.y();
	ctxt.rail_bottom += self.y();

//	push_translate( self.tl_corner() );
	if ( ctxt.use_left_rail )
		vline( out, point( ctxt.left_rail, ctxt.rail_top ), point( ctxt.left_rail, ctxt.rail_bottom ), "test" );
	if ( ctxt.use_right_rail )
		vline( out, point( ctxt.right_rail, ctxt.rail_top ), point( ctxt.right_rail, ctxt.rail_bottom ), "test" );
	if ( ctxt.use_left_rail || ctxt.use_right_rail )
		box( out, self.x(), self.y(), self.width()-1, self.height()-1, "test" );

	if ( self.width() > 0 && self.height() > 0 )
	{
//		box( out, self.x(), self.y(), self.width()-1, self.height()-1, "test" );
//		point p1 = self.l_anchor();
//		point p2 = self.r_anchor();
//		box( out, p1.x-2.5, p1.y-2.5, 2.5, 5, "lanchor" );
//		box( out, p2.x, p2.y-2.5, 2.5, 5, "ranchor" );
	}
//	pop_translate();

	if ( const ebnf *n = dynamic_cast<const ebnf*>( node ) )
	{
		ctxt.dir = NONE;
		above = false;
		ctxt.push_state();
//		svg_box &title = ctxt.data[n->title()];
		push_translate( self.tl_corner() );
		draw_svg( out, n->title(), ctxt, above );
		draw_svg( out, n->prods(), ctxt, above );
		draw_svg( out, n->comment(), ctxt, above );
		pop_translate();
		ctxt.pop_state();
	}
	else if ( const productions *n = dynamic_cast<const productions*>( node ) )
	{
		ctxt.dir = NONE;
		push_translate( self.tl_corner() );
		above = false;
		for ( size_t i = 0; i < n->size(); ++i )
		{
			ctxt.push_state();
			draw_svg( out, n->at( i ), ctxt, above );
			ctxt.pop_state();
		}
		pop_translate();
	}
	else if ( const production *n = dynamic_cast<const production*>( node ) )
	{
		above = false;
		svg_box &i = ctxt.data[n->id()];
		svg_box &e = ctxt.data[n->expr()];

		ctxt.push_state();
		push_translate( self.tl_corner() );
		{
			draw_svg( out, n->id(), ctxt, above );

			ctxt.dir = RIGHT;
			draw_svg( out, n->expr(), ctxt, above );
			hline( out, i.l_anchor().move( PADH, 0 ), e.l_anchor(), "line" );
		}
		point end = e.r_anchor().move( PADH*2, 0 );

		hline( out, e.r_anchor(), end, "line" );
		circle( out, end.x + CIRCLE/2, end.y, CIRCLE, "end" );
		pop_translate();
		ctxt.pop_state();
	}
	else if ( const expression *n = dynamic_cast<const expression*>( node ) )
	{
		push_translate( self.tl_corner() );
		if ( n->is_short() )
		{
			for ( size_t i = 0; i < n->size(); ++i )
			{
				ctxt.push_state();
				ctxt.dir = DOWN;
				ctxt.use_left_rail = false;
				ctxt.use_right_rail = false;
				bool new_above = false;
				draw_svg( out, n->at( i ), ctxt, new_above );
				ctxt.pop_state();
			}

			if ( ctxt.dir == RIGHT )
			{
				if ( above )
				{
					point start = self.tl_anchor().move( self.tl_corner().negate() );
					point end = self.tr_anchor().move( self.tl_corner().negate() );
					svg_box &s = ctxt.data[n->at(0)];
					svg_box &e = ctxt.data[n->at(n->size()-1)];

					path( out, DOWN, start, e.t_center(), DOWN, RADIUS, "line" );
					path( out, DOWN, s.b_center(), end, UP, RADIUS, "line" );
				}
				else
				{
					point start = self.l_anchor().move( self.tl_corner().negate() );
					point mid = self.br_corner().move( -RADIUS*2, -RADIUS ).move( self.tl_corner().negate() );
					point end = self.r_anchor().move( self.tl_corner().negate() );
					svg_box &s = ctxt.data[n->at(0)];
					svg_box &e = ctxt.data[n->at(n->size()-1)];

					path( out, RIGHT, start, e.t_center(), DOWN, RADIUS, "line" );
					path( out, DOWN, s.b_center(), mid, RIGHT, RADIUS, "line" );
					path( out, RIGHT, mid, end, RIGHT, RADIUS, "line" );
				}

				point top = self.r_anchor().move( self.tl_corner().negate() );
				point bot = self.bl_corner().move( RADIUS*2, -RADIUS ).move( self.tl_corner().negate() );

				for ( size_t i = 0; i < n->size(); ++i )
				{
					svg_box &b = ctxt.data[n->at(i)];
					if( i > 0 )
						path( out, DOWN, b.b_center(), point( b.b_center().x + RADIUS, bot.y ), RIGHT, RADIUS, "line" );
					if ( i+1 < n->size() )
						path( out, RIGHT, point( b.t_center().x - RADIUS, top.y ), b.t_center(), DOWN, RADIUS, "line" );
				}
			}
			else
			{
				if ( above )
				{
					point start = self.tr_anchor().move( self.tl_corner().negate() );
					point end = self.tl_anchor().move( self.tl_corner().negate() );
					svg_box &s = ctxt.data[n->at(n->size()-1)];
					svg_box &e = ctxt.data[n->at(0)];

					path( out, DOWN, start, e.t_center(), DOWN, RADIUS, "line" );
					path( out, DOWN, s.b_center(), end, UP, RADIUS, "line" );
				}
				else
				{
					point start = self.r_anchor().move( self.tl_corner().negate() );
					point mid = self.bl_corner().move( RADIUS*2, -RADIUS ).move( self.tl_corner().negate() );
					point end = self.l_anchor().move( self.tl_corner().negate() );
					svg_box &s = ctxt.data[n->at(n->size()-1)];
					svg_box &e = ctxt.data[n->at(0)];

					path( out, LEFT, start, e.t_center(), DOWN, RADIUS, "line" );
					path( out, DOWN, s.b_center(), mid, LEFT, RADIUS, "line" );
					path( out, LEFT, mid, end, LEFT, RADIUS, "line" );
				}

				point top = self.r_anchor().move( self.tl_corner().negate() );
				point bot = self.bl_corner().move( RADIUS*2, -RADIUS ).move( self.tl_corner().negate() );

				for ( size_t i = 0; i < n->size(); ++i )
				{
					svg_box &b = ctxt.data[n->at(i)];
					if( i > 0 )
						path( out, UP, b.t_center(), point( b.t_center().x + RADIUS, top.y ), RIGHT, RADIUS, "line" );
					if ( i+1 < n->size() )
						path( out, DOWN, b.b_center(), point( b.b_center().x - RADIUS, bot.y ), LEFT, RADIUS, "line" );
				}
			}
		}
		else
		{
			svg_box &s = ctxt.data[n->at(0)];
			svg_box &e = ctxt.data[n->at(n->size()-1)];

			bool new_above = false;
			ctxt.push_state();
			if ( !ctxt.use_left_rail )
			{
				ctxt.use_left_rail = true;
				ctxt.left_rail = -s.l_anchor().x + RADIUS;
				ctxt.rail_top = s.tl_anchor().y + self.y();
				ctxt.rail_bottom = e.tl_anchor().y + self.y();
			}
			if ( !ctxt.use_right_rail )
			{
				ctxt.use_right_rail = true;
				ctxt.right_rail = s.r_anchor().x - RADIUS;
				for ( size_t i = 1; i < n->size(); ++i )
					ctxt.right_rail = std::max( ctxt.data[n->at( i )].r_anchor().x - RADIUS, ctxt.right_rail );
				ctxt.rail_top = s.tl_anchor().y + self.y();
				ctxt.rail_bottom = e.tl_anchor().y;
			}

			for ( size_t i = 0; i < n->size(); ++i )
			{
				svg_box &box = ctxt.data[n->at( i )];
				ctxt.push_state();
				draw_svg( out, n->at( i ), ctxt, new_above );
				ctxt.pop_state();
			}
			ctxt.pop_state();

			Direction sd = RIGHT;
			Direction ed = RIGHT;
			point start = self.l_anchor().move( self.tl_corner().negate() );
			point end = self.r_anchor().move( self.tl_corner().negate() );
			if( ctxt.use_left_rail )
			{
				start.x = ctxt.left_rail;
				sd = DOWN;
			}

			if ( ctxt.use_right_rail )
			{
				end.x = ctxt.right_rail;
				ed = UP;
			}

			if ( above )
			{
				ctxt.use_left_rail = false;
				ctxt.use_right_rail = false;
				start = self.tl_anchor().move( self.tl_corner().negate() );
				end = self.tr_anchor().move( self.tl_corner().negate() );
				sd = DOWN;
				ed = UP;
			}

			if ( ctxt.use_left_rail )
				path( out, DOWN, point( start.x, e.l_anchor().y - RADIUS ), e.l_anchor(), RIGHT, RADIUS, "line" );
			else
				path( out, sd, start, e.l_anchor(), RIGHT, RADIUS, "line" );

			if ( ctxt.use_right_rail )
				path( out, RIGHT, e.r_anchor(), point( end.x, e.r_anchor().y - RADIUS ), ed, RADIUS, "line" );
			else
				path( out, RIGHT, e.r_anchor(), end, ed, RADIUS, "line" );

			if( !above )
			{
				if ( ctxt.use_left_rail )
					path( out, sd, start, s.l_anchor(), RIGHT, RADIUS, "line" );
				else
				{
					hline( out, start, s.l_anchor(), "line" );
					start.x += RADIUS;
				}

				if ( ctxt.use_right_rail )
					path( out, RIGHT, s.r_anchor(), point( end.x, s.r_anchor().y - RADIUS ), UP, RADIUS, "line" );
				else
				{
					hline( out, s.r_anchor(), end, "line" );
					end.x -= RADIUS;
				}
			}

			for ( size_t i = above ? 0 : 1; i < n->size()-1; ++i )
			{
				svg_box &b = ctxt.data[n->at(i)];
				path( out, DOWN, point( start.x, b.l_anchor().y - RADIUS ), b.l_anchor(), RIGHT, RADIUS, "line" );
				path( out, RIGHT, b.r_anchor(), point( end.x, b.r_anchor().y - RADIUS ), UP, RADIUS, "line" );
			}
		}
		pop_translate();
	}
	else if ( const term *n = dynamic_cast<const term*>( node ) )
	{
		above = false;
		push_translate( self.tl_corner() );
		ctxt.push_state();

		for ( size_t i = 0; i < n->size(); ++i )
		{
			svg_box &e = ctxt.data[n->at( i )];

			ctxt.push_state();
			ctxt.right_rail -= e.x();
			ctxt.left_rail -= e.x();
			ctxt.rail_bottom -= e.y();
			ctxt.rail_top -= e.y();
			if ( i > 0 )
				ctxt.use_left_rail = false;

			if ( i+1 < n->size() )
				ctxt.use_right_rail = false;

			draw_svg( out, n->at( i ), ctxt, above );
			ctxt.pop_state();
		}
		ctxt.pop_state();
		pop_translate();
	}
	else if ( const repetition *n = dynamic_cast<const repetition*>( node ) )
	{
		ctxt.push_state();
		ctxt.use_left_rail = false;
		ctxt.use_right_rail = false;
		push_translate( self.tl_corner() );
		{
			ctxt.reverse();
			above = true;
			draw_svg( out, n->expr(), ctxt, above );
		}
		pop_translate();
		ctxt.pop_state();

		if ( ctxt.dir == RIGHT )
			arrow_right( out, self.c_anchor().move( ARROW/2, 0 ), 0, ARROW, "line", "arrow" );
		else
			arrow_left( out, self.c_anchor().move( ARROW/2, 0 ), 0, ARROW, "line", "arrow" );
		hline( out, self.l_anchor(), self.r_anchor(), "line" );

		svg_box &e = ctxt.data[n->expr()];
		if ( above )
		{
			point lanch = e.tl_anchor().move( self.tl_corner() );
			point ranch = e.tr_anchor().move( self.tl_corner() );
			point start = point( ranch.x + RADIUS, self.l_anchor().y );
			point end = point( lanch.x - RADIUS, self.l_anchor().y );
			path( out, LEFT, start, lanch, DOWN, RADIUS, "line" );
			path( out, RIGHT, end, ranch, DOWN, RADIUS, "line" );
		}
		else
		{
			point lanch = e.l_anchor().move( self.tl_corner() );
			point ranch = e.r_anchor().move( self.tl_corner() );
			point start = point( ranch.x, self.l_anchor().y );
			point end = point( lanch.x, self.l_anchor().y );

			path( out, RIGHT, start, ranch, LEFT, RADIUS, "line" );
			path( out, LEFT, lanch, end, RIGHT, RADIUS, "line" );
			above = false;
		}
	}
	else if ( const onemore *n = dynamic_cast<const onemore*>( node ) )
	{
		ctxt.push_state();
		push_translate( self.tl_corner() );
		{
			draw_svg( out, n->expr(), ctxt, above );

			if ( n->sep() )
			{
				ctxt.reverse();
				draw_svg( out, n->sep(), ctxt, above );
			}
		}

		svg_box &e = ctxt.data[n->expr()];

		hline( out, point( 0, e.l_anchor().y ), e.l_anchor(), "line" );
		hline( out, e.r_anchor(), point( self.width(), e.r_anchor().y ), "line" );
		if ( n->sep() )
		{
			svg_box &s = ctxt.data[n->sep()];
			point lanch = s.l_anchor();
			point ranch = s.r_anchor();
			point start = e.r_anchor();
			point end = e.l_anchor();

			path( out, RIGHT, start, ranch, LEFT, RADIUS, "line" );
			path( out, LEFT, lanch, end, RIGHT, RADIUS, "line" );
		}
		else
		{
			float delta = ( e.r_anchor().y - e.tr_corner().y ) + PADH;
			if ( delta < RADIUS*2 )
				delta = RADIUS*2;
			point anch = e.r_anchor().move( 0, -delta );
			point start = e.r_anchor();
			point end = e.l_anchor();

			path( out, RIGHT, start, anch, LEFT, RADIUS, "line" );
			path( out, LEFT, anch, end, RIGHT, RADIUS, "line" );

			if ( ctxt.dir == RIGHT )
				arrow_left( out, anch.move( -e.width()/2, 0 ), 0, ARROW, "line", "arrow" );
			else
				arrow_right( out, anch.move( -e.width()/2, 0 ), 0, ARROW, "line", "arrow" );
		}
		pop_translate();
		ctxt.pop_state();
		above = false;
	}
	else if ( const optional *n = dynamic_cast<const optional*>( node ) )
	{
		ctxt.push_state();
		push_translate( self.tl_corner() );
		{
			above = true;
			svg_box &e = ctxt.data[n->expr()];
			ctxt.rail_top += e.y();
			ctxt.rail_bottom += e.y();
			draw_svg( out, n->expr(), ctxt, above );
		}
		pop_translate();
		ctxt.pop_state();

		svg_box &e = ctxt.data[n->expr()];
		point start = self.l_anchor();
		point end = self.r_anchor();

		hline( out, start, end, "line" );
		if ( ctxt.dir == RIGHT )
			arrow_right( out, self.c_anchor().move( ARROW/2, 0 ), 0, ARROW, "line", "arrow" );
		else
			arrow_left( out, self.c_anchor().move( ARROW/2, 0 ), 0, ARROW, "line", "arrow" );

		if ( above )
		{
			point lanch = e.tl_anchor().move( self.tl_corner() );
			point ranch = e.tr_anchor().move( self.tl_corner() );
			path( out, RIGHT, start, lanch, DOWN, RADIUS, "line" );
			path( out, UP, ranch, end, RIGHT, RADIUS, "line" );
		}
		else
		{
			point lanch = e.l_anchor().move( self.tl_corner() );
			point ranch = e.r_anchor().move( self.tl_corner() );

			if ( ctxt.use_left_rail )
				path( out, DOWN, point( ctxt.left_rail + self.x(), lanch.y - RADIUS ), lanch, RIGHT, RADIUS, "line" );
			else
				path( out, RIGHT, start, lanch, RIGHT, RADIUS, "line" );

			if ( ctxt.use_right_rail )
			{
				path( out, RIGHT, ranch, point( ctxt.right_rail + self.x(), std::min( ranch.y - RADIUS, ctxt.rail_bottom ) ), UP, RADIUS, "line" );
				circle( out, ctxt.right_rail + self.x(), ctxt.rail_bottom + self.y(), 5, "line" );
			}
			else
				path( out, RIGHT, ranch, end, RIGHT, RADIUS, "line" );
		}
		above = false;
	}
	else if ( const identifier *n = dynamic_cast<const identifier*>( node ) )
	{
		point p1 = self.tl_corner().move( PADH, PADV );
		point p2 = self.br_corner().move( -PADH, -PADV );

		switch ( ctxt.dir )
		{
			case NONE:
			case UP:
				break;

			case DOWN:
				p1 = p1.move( 0, ARROW );
				arrow_down( out, self.t_center(), PADV + ARROW, ARROW, "line", "arrow" );
				vline( out, self.b_center().move( 0, -PADV ), self.b_center(), "line" );
				break;
			case LEFT:
				p2 = p2.move( -ARROW, 0 );
				arrow_left( out, self.r_anchor(), PADH + ARROW, ARROW, "line", "arrow" );
				hline( out, self.l_anchor(), self.l_anchor().move( PADH, 0 ), "line" );
				break;
			case RIGHT:
				p1 = p1.move( ARROW, 0 );
				arrow_right( out, self.l_anchor(), PADH + ARROW, ARROW, "line", "arrow" );
				hline( out, self.r_anchor(), self.r_anchor().move( -PADH, 0 ), "line" );
				break;
		}

		if ( ctxt.dir != NONE )
			box( out, p1.x, p1.y, p2.x-p1.x, p2.y-p1.y, "box" );
		text_center( out, p1.x, p1.y, p2.x-p1.x, p2.y-p1.y-TEXT_PAD, n->value(), "ident" );
		above = false;
	}
	else if ( const literal *n = dynamic_cast<const literal*>( node ) )
	{
		point p1 = self.tl_corner().move( PADH, PADV );
		point p2 = self.br_corner().move( -PADH, -PADV );

		switch ( ctxt.dir )
		{
			case NONE:
			case UP:
				break;

			case DOWN:
				p1 = p1.move( 0, ARROW );
				arrow_down( out, self.t_center(), PADV + ARROW, ARROW, "line", "arrow" );
				vline( out, self.b_center().move( 0, -PADV ), self.b_center(), "line" );
				break;
			case LEFT:
				p2 = p2.move( -ARROW, 0 );
				arrow_left( out, self.r_anchor(), PADH + ARROW, ARROW, "line", "arrow" );
				hline( out, self.l_anchor(), self.l_anchor().move( PADH, 0 ), "line" );
				break;
			case RIGHT:
				p1 = p1.move( ARROW, 0 );
				arrow_right( out, self.l_anchor(), PADH + ARROW, ARROW, "line", "arrow" );
				hline( out, self.r_anchor(), self.r_anchor().move( -PADH, 0 ), "line" );
				break;
		}

		if ( ctxt.dir != NONE )
			round( out, p1.x, p1.y, p2.x-p1.x, p2.y-p1.y, "box" );
		text_center( out, p1.x, p1.y, p2.x-p1.x, p2.y-p1.y-TEXT_PAD, n->value(), "literal" );
		above = false;
	}
	else if ( const other *n = dynamic_cast<const other*>( node ) )
	{
		point p1 = self.tl_corner().move( PADH, PADV );
		point p2 = self.br_corner().move( -PADH, -PADV );

		switch ( ctxt.dir )
		{
			case NONE:
			case UP:
			case DOWN:
				break;
			case LEFT:
				p2 = p2.move( -ARROW, 0 );
				arrow_left( out, self.r_anchor(), PADH + ARROW, ARROW, "line", "arrow" );
				hline( out, self.l_anchor(), self.l_anchor().move( PADH, 0 ), "line" );
				break;
			case RIGHT:
				p1 = p1.move( ARROW, 0 );
				arrow_right( out, self.l_anchor(), PADH + ARROW, ARROW, "line", "arrow" );
				hline( out, self.r_anchor(), self.r_anchor().move( -PADH, 0 ), "line" );
				break;
		}
		if ( ctxt.dir != NONE )
			round( out, p1.x, p1.y, p2.x-p1.x, p2.y-p1.y, "box" );
		text_center( out, p1.x, p1.y, p2.x-p1.x, p2.y-p1.y-TEXT_PAD, n->value(), "other" );
		above = false;
	}
	ctxt.pop_state();
}

////////////////////////////////////////

void svg_generate( ostream &out, const node *e )
{
	const ebnf *n = dynamic_cast<const ebnf*>( e );
	if ( !n )
		throw runtime_error( "invalid ebnf node" );

	svg_context ctxt;
	bool above = false;
	compute_size( ctxt, n, above );

	svg_box &top = ctxt.data[n];
	out <<
		"<html><head>\n"
		"  <title>Grammar</title>\n"
		"  <meta http-equiv=\"Content-Type\" content=\"application/xhtml+xml; charset=UTF-8\"></meta>\n"
		"  <link type=\"text/css\" rel=\"stylesheet\" href=\"style.css\"></link>\n"
		"  <link type=\"text/css\" rel=\"stylesheet\" href=\"svg.css\"></link>\n"
		"</head><body>\n";

//	out << "<svg overflow=\"visible\" "
//		"xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" "
//		"xmlns:svg=\"http://www.w3.org/2000/svg\" "
//		"xmlns:xlink=\"http://www.w3.org/1999/xlink\" "
//		"width=\"" << top.width << "\" height=\"" << top.height << "\">\n";
	id_begin( out, top.x(), top.y(), top.width(), top.height(), "top" );
	draw_svg( out, n, ctxt, above );
	id_end( out );
//	out << "</svg>\n";
	out << "</body>\n";
}

////////////////////////////////////////

