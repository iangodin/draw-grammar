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

#include <string>
#include <vector>

using namespace std;

class node;

#define D_ParseNode_User node *

////////////////////////////////////////

class node
{
public:
	virtual ~node( void ) {}
};

////////////////////////////////////////

class literal : public node
{
public:
	literal( const char *start, const char *end, char quote )
		: _value( start, end ), _quote( quote )
	{
	}

	literal( char quote )
		: _quote( quote )
	{
	}
	inline char quote( void ) const { return _quote; }
	inline const string &value( void ) const { return _value; }

private:
	string _value;
	char _quote;
};

////////////////////////////////////////

class optional : public node
{
public:
	optional( node *expr )
		: _expr( expr )
	{
	}

	inline const node *expr( void ) const { return _expr; }

private:
	node *_expr;

};

////////////////////////////////////////

class onemore : public node
{
public:
	onemore( node *expr, node *sep = NULL )
		: _expr( expr ), _sep( sep )
	{
	}

	inline const node *expr( void ) const { return _expr; }
	inline const node *sep( void ) const { return _sep; }

private:
	node *_expr;
	node *_sep;

};

////////////////////////////////////////

class repetition : public node
{
public:
	repetition( node *expr )
		: _expr( expr )
	{
	}

	inline const node *expr( void ) const { return _expr; }

private:
	node *_expr;
};

////////////////////////////////////////

class term : public node
{
public:
	term( node *factors, node *n )
	{
		term *t = dynamic_cast<term*>( factors );
		if( t )
			*this = *t;
		else
			push_back( factors );
		push_back( n );
	}

	inline void push_back( node *n ) { _factors.push_back( n ); }
	inline size_t size( void ) const { return _factors.size(); }
	inline const node *at( int i ) const { return _factors.at( i ); }

private:
	vector<node *> _factors;
};

////////////////////////////////////////

class expression : public node
{
public:
	expression( node *exprs, node *n )
		: _short( true )
	{
		expression *e = dynamic_cast<expression*>( exprs );
		if( e )
			*this = *e;
		else
		{
			literal *lit = dynamic_cast<literal*>( exprs );
			if ( !lit || lit->value().size() > 3 )
				_short = false;
			push_back( exprs );
		}
		literal *lit = dynamic_cast<literal*>( n );
		if ( !lit || lit->value().size() > 3 )
			_short = false;
		push_back( n );
	}

	inline bool is_short( void ) const { return _short && _exprs.size() > 2; }
	inline void push_back( node *n ) { _exprs.push_back( n ); }
	inline size_t size( void ) const { return _exprs.size(); }
	inline const node *at( int i ) const { return _exprs.at( i ); }

private:
	bool _short;
	vector<node *> _exprs;
};

////////////////////////////////////////

class production : public node
{
public:
	production( node *id, node *expr )
		: _id( id ), _expr( expr )
	{
	}

	inline const node *id( void ) const { return _id; }
	inline const node *expr( void ) const { return _expr; }

private:
	node *_id;
	node *_expr;
};

////////////////////////////////////////

class productions : public node
{
public:
	productions( node *prods, node *n )
	{
		productions *p = dynamic_cast<productions*>( prods );
		if( p )
			*this = *p;
		else
			push_back( prods );
		push_back( n );
	}

	inline void push_back( node *n ) { _prods.push_back( n ); }
	inline size_t size( void ) const { return _prods.size(); }
	inline const node *at( int i ) const { return _prods.at( i ); }

private:
	vector<node *> _prods;
};

////////////////////////////////////////

class ebnf : public node
{
public:
	ebnf( node *title, node *prods, node *cmt )
		: _title( title ), _prods( prods ), _comment( cmt )
	{
	}

	inline const node *title( void ) const { return _title; }
	inline const node *prods( void ) const { return _prods; }
	inline const node *comment( void ) const { return _comment; }

private:
	node *_title;
	node *_prods;
	node *_comment;
};

////////////////////////////////////////

