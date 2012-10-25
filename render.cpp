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
#include <sstream>
#include <stdexcept>

#include "svg.h"
#include "node.h"
#include "print.h"
#include "render.h"
#include "draw.h"

#define TEXT_SIZE 24.F
#define TEXT_PAD 8.F
#define LINE_HEIGHT (TEXT_SIZE+TEXT_PAD)
#define CIRCLE 4.F
#define PADV 8.F
#define PADH 10.F
#define RADIUS 10.F
#define OVER_HEIGHT (RADIUS*2+PADH*2)
#define SPLIT_WIDTH (RADIUS*2+PADH)
#define ARROW_SIZE 10.F
#define SPACING 0.F
#define TEXT_RATIO 0.35F

////////////////////////////////////////

render_box &
compute_size( render_context &ctxt, const node *node, bool &above )
{
	ctxt.push_state();
	render_box &self = ctxt.data[node];
	self.init( PADH, LINE_HEIGHT/2 + PADV );
	if ( node == NULL )
		return self;

	if ( const ebnf *n = dynamic_cast<const ebnf*>( node ) )
	{
		ctxt.dir = NONE;
		ctxt.use_left_rail = ctxt.use_right_rail = false;
		above = false;
		render_box &title = compute_size( ctxt, n->title(), above );
		render_box &prods = compute_size( ctxt, n->prods(), above );
		render_box &comment = compute_size( ctxt, n->comment(), above );

		prods.move_to( title.bl_corner() );
		comment.move_to( prods.bl_corner() );

		self.include( title );
		self.include( prods );
		self.include( comment );
	}
	else if ( const productions *n = dynamic_cast<const productions*>( node ) )
	{
		ctxt.dir = NONE;
		ctxt.use_left_rail = ctxt.use_right_rail = false;
		above = false;
		for ( size_t i = 0; i < n->size(); ++i )
		{
			render_box &e = compute_size( ctxt, n->at( i ), above );
			e.move_to( self.bl_corner() );
			self.include( e );
		}
	}
	else if ( const production *n = dynamic_cast<const production*>( node ) )
	{
		ctxt.dir = RIGHT;
		ctxt.use_left_rail = ctxt.use_right_rail = false;
		above = false;

		render_box &id = compute_size( ctxt, n->id(), above );
		id.set_y_anchor( LINE_HEIGHT + PADH );

		render_box &expr = compute_size( ctxt, n->expr(), above );

		expr.move_l_anchor( id.r_anchor() );
		self.include( id );
		self.include( expr );
		self.include( expr.br_corner().move( PADH*3 + CIRCLE, 0 ) );

		point tl = self.tl_corner().negate();
		id.move_by( tl );
		expr.move_by( tl );
		self.move_by( tl );
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
				render_box &e = compute_size( ctxt, n->at( i ), new_above );
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
				ctxt.use_left_rail = true;
				ctxt.use_right_rail = true;
				for ( size_t i = 0; i < n->size(); ++i )
				{
					render_box &e = compute_size( ctxt, n->at( i ), new_above );
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
				}
			}
			ctxt.pop_state();

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

		point tl = self.tl_corner().negate();
		for ( size_t i = 0; i < n->size(); ++i )
		{
			render_box &e = ctxt.data[n->at(i)];
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
			render_box &e = compute_size( ctxt, n->at( i ), above );
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
			render_box &e = ctxt.data[n->at( i )];
			e.move_by( tl );
		}
		self.move_by( tl );

		render_box &e = ctxt.data[n->at( 0 )];
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
			render_box &e = compute_size( ctxt, n->expr(), above );

			self.set_y_anchor( PADV + ARROW_SIZE/2 );
			if ( above )
				self.include( point( RADIUS - PADH, PADV*2 + ARROW_SIZE ) );
			else
				self.include( point( RADIUS + PADH, PADV*2 + ARROW_SIZE ) );

			e.move_to( self.br_corner() );

			if ( above )
			{
				e.move_by( point( 0, std::max( 0.F, RADIUS - PADV - ARROW_SIZE/2 ) ) );
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

			render_box &e = compute_size( ctxt, n->expr(), above );
			e.move_l_anchor( point( RADIUS + PADH, self.l_anchor().y )  );

			point tl;

			if ( n->sep() )
			{
				ctxt.reverse();
				render_box &s = compute_size( ctxt, n->sep(), above );
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
				point p1( e.r_anchor().move( 0, -( RADIUS*2 + ARROW_SIZE/2 + PADH ) ) );
				point p2( e.tr_corner().move( 0, -( ARROW_SIZE + PADH ) ) );
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
			render_box &e = compute_size( ctxt, n->expr(), above );
			ctxt.pop_state();

			if ( above )
				self.include( point( RADIUS - PADV, std::max( PADV + ARROW_SIZE + PADV, PADV + ARROW_SIZE/2 + RADIUS ) ) );
			else
			{
				if ( ctxt.use_left_rail )
					self.include( point( 0, PADV*2 + ARROW_SIZE ) );
				else
					self.include( point( RADIUS*2, PADV*2 + ARROW_SIZE ) );
			}
			self.set_y_anchor( PADV + ARROW_SIZE/2 );

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
				self.set_width( float( n->value().size() + 2 ) * LINE_HEIGHT * TEXT_RATIO + ARROW_SIZE + PADH * 2.F );
				self.set_height( LINE_HEIGHT + PADV * 2.F );
				break;

			case UP:
			case DOWN:
				self.set_width( float( n->value().size() + 2 ) * LINE_HEIGHT * TEXT_RATIO + PADH * 2.F );
				self.set_height( LINE_HEIGHT + ARROW_SIZE + PADV * 2.F );
				break;
		}
		above = false;
	}
	else
		throw runtime_error( "unknown node type" );

	ctxt.pop_state();
	return self;
}

////////////////////////////////////////

void render( draw &dc, const node *node, render_context &ctxt, bool &above )
{
	render_box &self = ctxt.data[node];
	ctxt.push_state();

	if ( const ebnf *n = dynamic_cast<const ebnf*>( node ) )
	{
		ctxt.dir = NONE;
		above = false;
		ctxt.push_state();
		dc.push_translate( self.tl_corner() );
		if( n->title() )
			render( dc, n->title(), ctxt, above );
		render( dc, n->prods(), ctxt, above );
		if ( n->comment() )
			render( dc, n->comment(), ctxt, above );
		dc.pop_translate();
		ctxt.pop_state();
	}
	else if ( const productions *n = dynamic_cast<const productions*>( node ) )
	{
		ctxt.dir = NONE;
		dc.push_translate( self.tl_corner() );
		above = false;
		for ( size_t i = 0; i < n->size(); ++i )
		{
			ctxt.push_state();
			render( dc, n->at( i ), ctxt, above );
			ctxt.pop_state();
		}
		dc.pop_translate();
	}
	else if ( const production *n = dynamic_cast<const production*>( node ) )
	{
		above = false;
		render_box &i = ctxt.data[n->id()];
		render_box &e = ctxt.data[n->expr()];

		ctxt.push_state();
		dc.push_translate( self.tl_corner() );
		{
			render( dc, n->id(), ctxt, above );

			ctxt.dir = RIGHT;
			render( dc, n->expr(), ctxt, above );
			dc.hline( i.l_anchor().move( PADH, 0 ), e.l_anchor(), LINE );
		}
		point end = e.r_anchor().move( PADH*2, 0 );

		dc.hline( e.r_anchor(), end, LINE );
		dc.circle( end.x + CIRCLE/2, end.y, CIRCLE, END );
		dc.pop_translate();
		ctxt.pop_state();
	}
	else if ( const expression *n = dynamic_cast<const expression*>( node ) )
	{
		dc.push_translate( self.tl_corner() );
		ctxt.left_rail -= self.tl_corner().x;
		ctxt.right_rail -= self.tl_corner().x;
		ctxt.rail_top -= self.tl_corner().y;
		ctxt.rail_bottom -= self.tl_corner().y;

		if ( n->is_short() )
		{
			for ( size_t i = 0; i < n->size(); ++i )
			{
				ctxt.push_state();
				ctxt.dir = DOWN;
				ctxt.use_left_rail = false;
				ctxt.use_right_rail = false;
				bool new_above = false;
				render( dc, n->at( i ), ctxt, new_above );
				ctxt.pop_state();
			}

			if ( ctxt.dir == RIGHT )
			{
				if ( above )
				{
					point start = self.tl_anchor().move( self.tl_corner().negate() );
					point end = self.tr_anchor().move( self.tl_corner().negate() );
					render_box &s = ctxt.data[n->at(0)];
					render_box &e = ctxt.data[n->at(n->size()-1)];

					dc.path( DOWN, start, e.t_center(), DOWN, RADIUS, LINE );
					dc.path( DOWN, s.b_center(), end, UP, RADIUS, LINE );
				}
				else
				{
					point start = self.l_anchor().move( self.tl_corner().negate() );
					point mid = self.br_corner().move( -RADIUS*2, -RADIUS ).move( self.tl_corner().negate() );
					point end = self.r_anchor().move( self.tl_corner().negate() );
					render_box &s = ctxt.data[n->at(0)];
					render_box &e = ctxt.data[n->at(n->size()-1)];

					dc.path( RIGHT, start, e.t_center(), DOWN, RADIUS, LINE );
					dc.path( DOWN, s.b_center(), mid, RIGHT, RADIUS, LINE );
					dc.path( RIGHT, mid, end, RIGHT, RADIUS, LINE );
				}

				point top = self.r_anchor().move( self.tl_corner().negate() );
				point bot = self.bl_corner().move( RADIUS*2, -RADIUS ).move( self.tl_corner().negate() );

				for ( size_t i = 0; i < n->size(); ++i )
				{
					render_box &b = ctxt.data[n->at(i)];
					if( i > 0 )
						dc.path( DOWN, b.b_center(), point( b.b_center().x + RADIUS, bot.y ), RIGHT, RADIUS, LINE );
					if ( i+1 < n->size() )
						dc.path( RIGHT, point( b.t_center().x - RADIUS, top.y ), b.t_center(), DOWN, RADIUS, LINE );
				}
			}
			else
			{
				if ( above )
				{
					point start = self.tr_anchor().move( self.tl_corner().negate() );
					point end = self.tl_anchor().move( self.tl_corner().negate() );
					render_box &s = ctxt.data[n->at(n->size()-1)];
					render_box &e = ctxt.data[n->at(0)];

					dc.path( DOWN, start, e.t_center(), DOWN, RADIUS, LINE );
					dc.path( DOWN, s.b_center(), end, UP, RADIUS, LINE );
				}
				else
				{
					point start = self.r_anchor().move( self.tl_corner().negate() );
					point mid = self.bl_corner().move( RADIUS*2, -RADIUS ).move( self.tl_corner().negate() );
					point end = self.l_anchor().move( self.tl_corner().negate() );
					render_box &s = ctxt.data[n->at(n->size()-1)];
					render_box &e = ctxt.data[n->at(0)];

					dc.path( LEFT, start, e.t_center(), DOWN, RADIUS, LINE );
					dc.path( DOWN, s.b_center(), mid, LEFT, RADIUS, LINE );
					dc.path( LEFT, mid, end, LEFT, RADIUS, LINE );
				}

				point top = self.r_anchor().move( self.tl_corner().negate() );
				point bot = self.bl_corner().move( RADIUS*2, -RADIUS ).move( self.tl_corner().negate() );

				for ( size_t i = 0; i < n->size(); ++i )
				{
					render_box &b = ctxt.data[n->at(i)];
					if( i > 0 )
						dc.path( UP, b.t_center(), point( b.t_center().x + RADIUS, top.y ), RIGHT, RADIUS, LINE );
					if ( i+1 < n->size() )
						dc.path( DOWN, b.b_center(), point( b.b_center().x - RADIUS, bot.y ), LEFT, RADIUS, LINE );
				}
			}
		}
		else
		{
			render_box &s = ctxt.data[n->at(0)];
			render_box &e = ctxt.data[n->at(n->size()-1)];

			bool new_above = false;
			ctxt.push_state();
			if ( !ctxt.use_left_rail )
			{
				ctxt.use_left_rail = true;
				ctxt.left_rail = RADIUS;
				ctxt.rail_top = s.l_anchor().y + RADIUS;
				ctxt.rail_bottom = e.r_anchor().y - RADIUS;
			}
			if ( !ctxt.use_right_rail )
			{
				ctxt.use_right_rail = true;
				ctxt.right_rail = s.r_anchor().x + RADIUS;
				for ( size_t i = 1; i < n->size(); ++i )
					ctxt.right_rail = std::max( ctxt.data[n->at( i )].r_anchor().x + RADIUS, ctxt.right_rail );
				ctxt.rail_top = s.l_anchor().y + RADIUS;
				ctxt.rail_bottom = e.r_anchor().y - RADIUS;
			}

			for ( size_t i = 0; i < n->size(); ++i )
			{
				ctxt.push_state();
				render( dc, n->at( i ), ctxt, new_above );
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
				dc.path( DOWN, point( start.x, e.l_anchor().y - RADIUS ), e.l_anchor(), RIGHT, RADIUS, LINE );
			else
				dc.path( sd, start, e.l_anchor(), RIGHT, RADIUS, LINE );

			if ( ctxt.use_right_rail )
				dc.path( RIGHT, e.r_anchor(), point( end.x, std::min( ctxt.rail_bottom, e.r_anchor().y - RADIUS ) ), ed, RADIUS, LINE );
			else
				dc.path( RIGHT, e.r_anchor(), end, ed, RADIUS, LINE );

			if( !above )
			{
				if ( ctxt.use_left_rail )
					dc.path( sd, start, s.l_anchor(), RIGHT, RADIUS, LINE );
				else
				{
					dc.hline( start, s.l_anchor(), LINE );
					start.x += RADIUS;
				}

				if ( ctxt.use_right_rail )
				{
					dc.hline( s.r_anchor(), point( self.r_anchor().x - self.tl_corner().x, s.r_anchor().y - RADIUS ), LINE );
				}
				else
				{
					dc.hline( s.r_anchor(), end, LINE );
					end.x -= RADIUS;
				}
			}

			for ( size_t i = above ? 0 : 1; i < n->size()-1; ++i )
			{
				render_box &b = ctxt.data[n->at(i)];
				dc.path( DOWN, point( start.x, b.l_anchor().y - RADIUS ), b.l_anchor(), RIGHT, RADIUS, LINE );
				dc.path( RIGHT, b.r_anchor(), point( end.x, b.r_anchor().y - RADIUS ), UP, RADIUS, LINE );
			}
		}
		dc.pop_translate();
	}
	else if ( const term *n = dynamic_cast<const term*>( node ) )
	{
		above = false;

		dc.push_translate( self.tl_corner() );
		ctxt.push_state();
		ctxt.left_rail -= self.tl_corner().x;
		ctxt.right_rail -= self.tl_corner().x;
		ctxt.rail_top -= self.tl_corner().y;
		ctxt.rail_bottom -= self.tl_corner().y;

		for ( size_t i = 0; i < n->size(); ++i )
		{
			ctxt.push_state();
			if ( i > 0 )
				ctxt.use_left_rail = false;

			if ( i+1 < n->size() )
				ctxt.use_right_rail = false;

			render( dc, n->at( i ), ctxt, above );
			ctxt.pop_state();
		}
		ctxt.pop_state();
		dc.pop_translate();
	}
	else if ( const repetition *n = dynamic_cast<const repetition*>( node ) )
	{
		ctxt.push_state();
		ctxt.use_left_rail = false;
		ctxt.use_right_rail = false;
		dc.push_translate( self.tl_corner() );
		{
			ctxt.reverse();
			above = true;
			render( dc, n->expr(), ctxt, above );
		}
		dc.pop_translate();
		ctxt.pop_state();

		if ( ctxt.dir == RIGHT )
			dc.arrow_right( self.c_anchor().move( ARROW_SIZE/2, 0 ), 0, ARROW_SIZE, LINE, ARROW );
		else
			dc.arrow_left( self.c_anchor().move( ARROW_SIZE/2, 0 ), 0, ARROW_SIZE, LINE, ARROW );
		dc.hline( self.l_anchor(), self.r_anchor(), LINE );

		render_box &e = ctxt.data[n->expr()];
		if ( above )
		{
			point lanch = e.tl_anchor().move( self.tl_corner() );
			point ranch = e.tr_anchor().move( self.tl_corner() );
			point start = point( ranch.x + RADIUS, self.l_anchor().y );
			point end = point( lanch.x - RADIUS, self.l_anchor().y );
			dc.path( LEFT, start, lanch, DOWN, RADIUS, LINE );
			dc.path( RIGHT, end, ranch, DOWN, RADIUS, LINE );
		}
		else
		{
			point lanch = e.l_anchor().move( self.tl_corner() );
			point ranch = e.r_anchor().move( self.tl_corner() );
			point start = point( ranch.x, self.l_anchor().y );
			point end = point( lanch.x, self.l_anchor().y );

			dc.path( RIGHT, start, ranch, LEFT, RADIUS, LINE );
			dc.path( LEFT, lanch, end, RIGHT, RADIUS, LINE );
			above = false;
		}
	}
	else if ( const onemore *n = dynamic_cast<const onemore*>( node ) )
	{
		ctxt.push_state();
		dc.push_translate( self.tl_corner() );
		ctxt.left_rail -= self.tl_corner().x;
		ctxt.right_rail -= self.tl_corner().x;
		ctxt.rail_top -= self.tl_corner().y;
		ctxt.rail_bottom -= self.tl_corner().y;
		{
			render( dc, n->expr(), ctxt, above );

			if ( n->sep() )
			{
				ctxt.reverse();
				render( dc, n->sep(), ctxt, above );
			}
		}

		render_box &e = ctxt.data[n->expr()];

		dc.hline( point( 0, e.l_anchor().y ), e.l_anchor(), LINE );
		dc.hline( e.r_anchor(), point( self.width(), e.r_anchor().y ), LINE );
		if ( n->sep() )
		{
			render_box &s = ctxt.data[n->sep()];
			point lanch = s.l_anchor();
			point ranch = s.r_anchor();
			point start = e.r_anchor();
			point end = e.l_anchor();

			dc.path( RIGHT, start, ranch, LEFT, RADIUS, LINE );
			dc.path( LEFT, lanch, end, RIGHT, RADIUS, LINE );
		}
		else
		{
			float delta = ( e.r_anchor().y - e.tr_corner().y ) + PADH;
			if ( delta < RADIUS*2 )
				delta = RADIUS*2;
			point anch = e.r_anchor().move( 0, -delta );
			point start = e.r_anchor();
			point end = e.l_anchor();

			dc.path( RIGHT, start, anch, LEFT, RADIUS, LINE );
			dc.path( LEFT, anch, end, RIGHT, RADIUS, LINE );

			if ( ctxt.dir == RIGHT )
				dc.arrow_left( anch.move( -e.width()/2, 0 ), 0, ARROW_SIZE, LINE, ARROW );
			else
				dc.arrow_right( anch.move( -e.width()/2, 0 ), 0, ARROW_SIZE, LINE, ARROW );
		}
		dc.pop_translate();
		ctxt.pop_state();
		above = false;
	}
	else if ( const optional *n = dynamic_cast<const optional*>( node ) )
	{
		ctxt.push_state();
		dc.push_translate( self.tl_corner() );
		ctxt.left_rail -= self.tl_corner().x;
		ctxt.right_rail -= self.tl_corner().x;
		ctxt.rail_top -= self.tl_corner().y;
		ctxt.rail_bottom -= self.tl_corner().y;
		{
			above = true;
			render( dc, n->expr(), ctxt, above );
		}
		dc.pop_translate();
		ctxt.pop_state();

		render_box &e = ctxt.data[n->expr()];
		point start = self.l_anchor();
		point end = self.r_anchor();

		dc.hline( start, end, LINE );
		if ( ctxt.dir == RIGHT )
			dc.arrow_right( self.c_anchor().move( ARROW_SIZE/2, 0 ), 0, ARROW_SIZE, LINE, ARROW );
		else
			dc.arrow_left( self.c_anchor().move( ARROW_SIZE/2, 0 ), 0, ARROW_SIZE, LINE, ARROW );

		if ( above )
		{
			point lanch = e.tl_anchor().move( self.tl_corner() );
			point ranch = e.tr_anchor().move( self.tl_corner() );
			dc.path( RIGHT, start, lanch, DOWN, RADIUS, LINE );
			dc.path( UP, ranch, end, RIGHT, RADIUS, LINE );
		}
		else
		{
			point lanch = e.l_anchor().move( self.tl_corner() );
			point ranch = e.r_anchor().move( self.tl_corner() );

			if ( ctxt.use_left_rail )
				dc.path( DOWN, point( ctxt.left_rail, std::min( ctxt.rail_bottom, lanch.y - RADIUS ) ), lanch, RIGHT, RADIUS, LINE );
			else
				dc.path( RIGHT, start, lanch, RIGHT, RADIUS, LINE );

			if ( ctxt.use_right_rail )
				dc.path( RIGHT, ranch, point( ctxt.right_rail, std::min( ranch.y - RADIUS, ctxt.rail_bottom ) ), UP, RADIUS, LINE );
			else
				dc.path( RIGHT, ranch, end, RIGHT, RADIUS, LINE );
		}
		above = false;
	}
	else if ( const literal *n = dynamic_cast<const literal*>( node ) )
	{
		point p1 = self.tl_corner().move( PADH, PADV );
		point p2 = self.br_corner().move( -PADH, -PADV );

		Class cl = LITERAL;
		switch ( n->quote() )
		{
			case '\0': cl = IDENTIFIER; break;
			case '\"': cl = LITERAL; break;
			case '*': cl = OTHER; break;
			case 'T': cl = TITLE; break;
		}

		switch ( ctxt.dir )
		{
			case NONE:
				if ( cl != TITLE )
					cl = PRODUCTION;
				break;

			case UP:
				break;

			case DOWN:
				p1 = p1.move( 0, ARROW_SIZE );
				dc.arrow_down( self.t_center(), PADV + ARROW_SIZE, ARROW_SIZE, LINE, ARROW );
				dc.vline( self.b_center().move( 0, -PADV ), self.b_center(), LINE );
				break;
			case LEFT:
				p2 = p2.move( -ARROW_SIZE, 0 );
				dc.arrow_left( self.r_anchor(), PADH + ARROW_SIZE, ARROW_SIZE, LINE, ARROW );
				dc.hline( self.l_anchor(), self.l_anchor().move( PADH, 0 ), LINE );
				break;
			case RIGHT:
				p1 = p1.move( ARROW_SIZE, 0 );
				dc.arrow_right( self.l_anchor(), PADH + ARROW_SIZE, ARROW_SIZE, LINE, ARROW );
				dc.hline( self.r_anchor(), self.r_anchor().move( -PADH, 0 ), LINE );
				break;
		}

		switch ( cl )
		{
			case PRODUCTION:
			case TITLE:
				dc.text( p1.x + PADH, p1.y, p2.x-p1.x, p2.y-p1.y-TEXT_PAD, n->value(), cl );
				break;

			case IDENTIFIER:
				dc.box( p1.x, p1.y, p2.x-p1.x, p2.y-p1.y, cl );
				dc.text_center( p1.x, p1.y, p2.x-p1.x, p2.y-p1.y-TEXT_PAD, n->value(), cl );
				break;

			default:
				dc.round( p1.x, p1.y, p2.x-p1.x, p2.y-p1.y, cl );
				dc.text_center( p1.x, p1.y, p2.x-p1.x, p2.y-p1.y-TEXT_PAD, n->value(), cl );
				break;
		}
		above = false;
	}
	else
	{
		stringstream tmp;
		tmp << (void*)node << ' ' << *node;
		throw runtime_error( string( "Unknown node type: " ) + tmp.str() );
	}
	ctxt.pop_state();
}

////////////////////////////////////////

void render( draw &dc, const node *e )
{
	const ebnf *n = dynamic_cast<const ebnf*>( e );
	if ( !n )
		throw runtime_error( "invalid ebnf node" );

	render_context ctxt;
	bool above = false;
	compute_size( ctxt, n, above );

	render_box &top = ctxt.data[n];
	dc.id_begin( top.x(), top.y(), top.width(), top.height(), "top" );
	render( dc, n, ctxt, above );
	dc.id_end();
}

////////////////////////////////////////

