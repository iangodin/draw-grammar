
Include( SourceFile() )

srcs = {
	"main.cpp",
	"print.cpp",
	DParse( "ebnf.g" ),
}

Executable( "ebnf2svg", Compile( srcs ), LinkSys( "dparse" ) );

