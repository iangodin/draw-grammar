
{

#include "node.h"

}

ebnf:
	title '{' productions '}' comment
		{
			$$ = new ebnf( $0, $2, $4 );
		};

productions:
	production
		{
			$$ = new productions( $0, NULL );
		}|
	productions production
		{
			$$ = new productions( $1, $0 );
		}|;

production:
	identifier '=' expression '.'
		{
			$$ = new production( $0, $2 );
		};

expression:
	term
	{
		$$ = new expression( $0, NULL );
	}|
	expression '|' term
	{
		$$ = new expression( $2, $0 );
	};

term:
	factor
	{
		$$ = new term( $0, NULL );
	}|
	term factor
	{
		$$ = new term( $1, $0 );
	};

factor:
	identifier
		{
			$$ = $0;
		}|
	literal
		{
			$$ = $0;
		}|
	'[' expression ']'
		{
			$$ = new optional( $1 );
		}|
	'(' expression ')'
		{
			$$ = new group( $1 );
		}|
	'{' expression '}'
		{
			$$ = new repetition( $1 );
		};

identifier:
	"[a-zA-Z][a-zA-Z0-9_]*"
		{
			$$ = new identifier( $n0.start_loc.s, $n0.end );
		};

title:
	// nothing
		{
			$$ = NULL;
		}|
	literal
		{
			$$ = $0;
		};

comment:
	// nothing
		{
			$$ = NULL;
		}|
	literal
		{
			$$ = $0;
		};

literal:
	"\'[^']+\'"
		{
			$$ = new literal( $n0.start_loc.s+1, $n0.end-1, true );
		}|
	"\"[^\"]+\""
		{
			$$ = new literal( $n0.start_loc.s+1, $n0.end-1, false );
		};

