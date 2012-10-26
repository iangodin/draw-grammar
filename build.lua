
Include( SourceFile() )

srcs = {
	"main.cpp",
	"print.cpp",
	"draw.cpp",
	"svg.cpp",
	"tikz.cpp",
	"html.cpp",
	"render.cpp",
	DParse( "grammar.g" ),
}

Executable( "draw_grammar", Compile( srcs ), LinkSys( "dparse" ) );

