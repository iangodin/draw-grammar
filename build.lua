
Include( SourceFile() )

srcs = {
	"main.cpp",
	"print.cpp",
	"svg.cpp",
	DParse( "ebnf.g" ),
}

Executable( "ebnf2svg", Compile( srcs ), LinkSys( "dparse" ) );

