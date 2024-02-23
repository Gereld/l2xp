dofileopt "default.lua"

newoption {
    trigger = "to",
    value   = "path",
    description = "Set the output location for the generated files"
}

workspace "l2xp" 
	location		( _OPTIONS["to"] )
	configurations	{ "Release", "Debug" }
	platforms		{ "x64" }
	
	filter { "configurations:Debug" }
		optimize 	"Debug"
		symbols 	"On"

	filter { "configurations:Release" }
		optimize 		"Full"
		symbols 		"Off"
		staticruntime	"On"

project "l2xp"
	kind			"WindowedApp"
	entrypoint		"WinMainCRTStartup"
	cppdialect		"C++17"
	removebuildoptions { "/std:c++latest" }
	characterset	"MBCS"	

	links 			{ "Gx", "wex", "wpcap", "Ws2_32" }
	includedirs		{ ".", "src", "res", "3rdparty/npcap-sdk-1.13/include" }
	libdirs			{ "3rdparty/npcap-sdk-1.13/lib/x64" }

	files { "src/**.h", "src/**.hpp", "src/**.cpp" }

	pchheader		"stdafx.hpp"
	pchsource		"stdafx.cpp"
	forceincludes 	{ "stdafx.hpp" }
	files 			{ "stdafx.hpp", "stdafx.cpp" }
