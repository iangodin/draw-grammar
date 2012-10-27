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

#include <stdexcept>
#include "print.h"

////////////////////////////////////////

ostream &operator<<( ostream &out, const node &node )
{
	if ( &node == NULL )
		return out;

	if ( const grammar *n = dynamic_cast<const grammar*>( &node ) )
	{
		out << *n->title() << "\n{\n" << *n->prods() << "}\n";
	}
	else if ( const productions *n = dynamic_cast<const productions*>( &node ) )
	{
		for ( size_t i = 0; i < n->size(); ++i )
			out << *( n->at( i ) );
	}
	else if ( const production *n = dynamic_cast<const production*>( &node ) )
	{
		out << '\t' << *n->id() << " = " << *n->expr() << " .\n";
	}
	else if ( const expression *n = dynamic_cast<const expression*>( &node ) )
	{
		for ( size_t i = 0; i < n->size(); ++i )
		{
			if ( i > 0 )
				out << " | ";
			out << *( n->at( i ) );
		}
	}
	else if ( const term *n = dynamic_cast<const term*>( &node ) )
	{
		for ( size_t i = 0; i < n->size(); ++i )
		{
			if ( i > 0 )
				out << ' ';
			out << *( n->at( i ) );
		}
	}
	else if ( const repetition *n = dynamic_cast<const repetition*>( &node ) )
	{
		out << "{ " << *n->expr() << " }";
	}
	else if ( const onemore *n = dynamic_cast<const onemore*>( &node ) )
	{
		out << "< " << *n->expr() << " >";
	}
	else if ( const optional *n = dynamic_cast<const optional*>( &node ) )
	{
		out << "[ " << *n->expr() << " ]";
	}
	else if ( const literal *n = dynamic_cast<const literal*>( &node ) )
	{
		if ( n->quote() )
			out << n->quote() << n->value() << n->quote();
		else
			out << n->value();
	}
	else
		throw runtime_error( "unknown node type" );

	return out;
}

////////////////////////////////////////

