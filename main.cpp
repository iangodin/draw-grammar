
#include <iostream>
#include <string.h>
#include <string>
#include <stdexcept>

#include "node.h"
#include "print.h"
#include "svg.h"
#include "render.h"
#include <dparse.h>

using namespace std;

extern D_ParserTables parser_tables_gram;

////////////////////////////////////////

void
syntax( struct D_Parser *parser )
{
	size_t lnum = parser->loc.line;
	size_t col = parser->loc.col;
	char *tmp = parser->loc.s - col;
	char *end = strchrnul( tmp, '\n' );
	string text( tmp, size_t( end - tmp ) );
	string indent( text );
	for ( char &c: indent )
	{
		if ( c != ' ' && c != '\t' )
			c = ' ';
	}
	string indent2( indent );
	if ( col < indent.size() )
	{
		indent.replace( col, 1, "\033[1;31m⬇\033[0;39m" );
		indent2.replace( col, 1, "\033[1;31m⬆\033[0;39m" );
	}
	cerr << "Error at line " << lnum << "  col " << col << ":\n" << indent << '\n' << text << '\n' << indent2 << "\n\n";
	parser->error_recovery = 0;
}

////////////////////////////////////////

struct D_ParseNode *
ambiguous( struct D_Parser *parser, int n, struct D_ParseNode **v )
{
	cerr << "Ambiguous!!!\n";
	for ( int i = 0; i < n; ++i )
		cerr << "Parse " << i << ' ' << v[i]->user << ":\n" << *(v[i]->user);
	cerr << endl;
	return NULL;
}

////////////////////////////////////////

node *
parse( istream &in )
{
	string str( (istreambuf_iterator<char>( in )), istreambuf_iterator<char>() );

	D_Parser *p = new_D_Parser( &parser_tables_gram, sizeof( node * ) );
	p->syntax_error_fn = &syntax;
	p->ambiguity_fn = &ambiguous;
	p->error_recovery = 1;
	p->save_parse_tree = 1;
	p->syntax_errors = 0;
	p->loc.line = 1;
	p->loc.col = 1;

	node *ret = NULL;

	D_ParseNode *parsed = NULL;
	parsed = dparse( p, &str[0], str.size() );

	if ( parsed && !p->syntax_errors )
	{
		if ( !parsed->user )
			throw runtime_error( "no abstract syntax tree" );
		ret = parsed->user;
		free_D_ParseNode( p, parsed );
	}
	else if ( !p->syntax_errors )
	{
		cerr << "Unknown error! " << (void *)parsed << endl;
		if ( parsed )
			free_D_ParseNode( p, parsed );
	}

	free_D_Parser( p );

	return ret;
}

////////////////////////////////////////

int main( int argc, char *argv[] )
{
	node *node = parse( cin );
//	cout << *node << endl;
	draw_svg dc( cout );
	render( dc, node );
}


