
{

#include "node.h"

}

start:
	ebnf
	{
		$$ = $0;
	}|
	production
	{
		$$ = $0;
	};

ebnf:
	title '{' productions '}' comment
		{
			$$ = new ebnf( $0, $2, $4 );
		};

productions:
	production
		{
			$$ = $0;
		}|
	productions production
		{
			$$ = new productions( $0, $1 );
		}|;

production:
	identifier '=' expression '.'
		{
			$$ = new production( $0, $2 );
		};

expression:
	term
	{
		$$ = $0;
	}|
	expression '|' term
	{
		$$ = new expression( $0, $2 );
	};

term:
	factor
	{
		$$ = $0;
	}|
	term factor
	{
		$$ = new term( $0, $1 );
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
			$$ = $1;
		}|
	'{' expression '}'
		{
			$$ = new repetition( $1 );
		}|
	'<' expression '>'
		{
			$$ = new onemore( $1 );
		}|
	'<' expression '~' expression '>'
		{
			$$ = new onemore( $1, $3 );
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
		}|
	"\?[^\?]+\?"
		{
			$$ = new other( $n0.start_loc.s+1, $n0.end-1 );
		};

