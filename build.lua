
Include( SourceFile() )

srcs = {
	"main.cpp",
	"print.cpp",
	"draw.cpp",
	"svg.cpp",
	"render.cpp",
	DParse( "ebnf.g" ),
}

Executable( "draw_ebnf", Compile( srcs ), LinkSys( "dparse" ) );

