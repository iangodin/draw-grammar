
DefaultBuildType( "build" )

BuildDir( "build", "build" )
BuildDir( "debug", "debug" )
BuildDir( "release", "release" )

if System() == "Darwin" then
	Variable( "cxx", "clang++" )
	Variable( "cc", "clang" )
	Variable( "ld", "clang++" )
else
	Variable( "cxx", "g++" )
	Variable( "cc", "gcc" )
	Variable( "ld", "g++" )
end
Variable( "binld", "ld" )
Variable( "ar", "ar" )
Variable( "moc", "moc-qt4" )
Variable( "rcc", "rcc" )
Variable( "luac", "luac" )
Variable( "dparser", "make_dparser" )

CFlags( "-msse", "-msse2", "-msse3" )
CFlags( "-flax-vector-conversions" )
CFlags( "-fPIC" )
CXXFlags( "-msse", "-msse2", "-msse3" )
CXXFlags( "-flax-vector-conversions" )
CXXFlags( "--std=c++0x" )
if System() == "Darwin" then
	CXXFlags( "--stdlib=libc++" )
	LDFlags( "--std=c++0x" )
	LDFlags( "--stdlib=libc++" )
end
Variable( "dflags", "-Xcpp" )

-- C/C++ warnings
Warning( "all", "extra", "no-unused-parameter", "init-self", "comment", "cast-align", "switch", "format", "multichar", "missing-braces", "parentheses", "pointer-arith", "sign-compare", "return-type", "write-strings", "cast-align" )

-- C warnings
CWarning( "unused" )

-- C++ warnings
CXXWarning( "unused", "overloaded-virtual", "no-ctor-dtor-privacy", "non-virtual-dtor", "sign-promo", "missing-field-initializers" )
if System() ~= "Darwin" then
	CXXWarning( "pmf-conversions" )
end

if Building( "debug" ) then
	CFlags( "-ggdb", "-fvar-tracking-uninit", "-fbounds-check", "-fcheck-data-deps" )
--	Library = SharedLibrary
--	LinkLibs = LinkSharedLibs
	Library = StaticLibrary
	LinkLibs = LinkStaticLibs
end

if Building( "build" ) then
	CFlags( "-O3" )
	Definition( "NDEBUG" )
	Library = StaticLibrary
	LinkLibs = LinkStaticLibs
end

if Building( "release" ) then
	CFlags( "-O3" )
	Definition( "NDEBUG" )
	Variable( "luaflags", "-s" ) -- Strip lua files
	Definition( "QT_NO_DEBUG" )
	Library = StaticLibrary
	LinkLibs = LinkStaticLibs
end

Definition( "__STDC_LIMIT_MACROS", "_FILE_OFFSET_BITS=64" )

