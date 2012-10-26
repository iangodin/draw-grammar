
Include( SourceFile() )

srcs = {
	"main.cpp",
	"print.cpp",
	"draw.cpp",
	"svg.cpp",
	"tikz.cpp",
	"html.cpp",
	"render.cpp",
	DParse( "ebnf.g" ),
}

Executable( "draw_ebnf", Compile( srcs ), LinkSys( "dparse" ) );

