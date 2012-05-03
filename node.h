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
	literal( const char *start, const char *end, bool single )
		: _value( start, end ), _single( single )
	{
	}

	inline bool single( void ) const { return _single; }
	inline const string &value( void ) const { return _value; }

private:
	string _value;
	bool _single;
};

////////////////////////////////////////

class identifier : public node
{
public:
	identifier( const char *start, const char *end )
		: _value( start, end )
	{
	}

	inline const string &value( void ) const { return _value; }

private:
	string _value;
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

class group : public node
{
public:
	group( node *expr )
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
	term( node *v, node *p )
		: _value( v ), _prev( p )
	{
	}

	inline const node *value( void ) const { return _value; }
	inline const node *prev( void ) const { return _prev; }

private:
	node *_value;
	node *_prev;
};

////////////////////////////////////////

class expression : public node
{
public:
	expression( node *v, node *p )
		: _value( v ), _prev( p )
	{
	}

	inline const node *value( void ) const { return _value; }
	inline const node *prev( void ) const { return _prev; }

private:
	node *_value;
	node *_prev;
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
	productions( node *v, node *p )
		: _value( v ), _prev( p )
	{
	}

	inline const node *value( void ) const { return _value; }
	inline const node *prev( void ) const { return _prev; }

private:
	node *_value;
	node *_prev;
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

