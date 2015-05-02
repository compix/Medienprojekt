dofile "premake/actions/netbeans5/_netbeans.lua"
dofile "premake/actions/netbeans5/netbeans_cpp.lua"

-- Prevent from script error when no action is given
if not _ACTION then
	printf "Error: No action defined!"
	return
end

-- Check for supported action
isNetbeans = _ACTION == "netbeans"
isVisualStudio = _ACTION == "vs2013"
if not isNetbeans and not isVisualStudio then
	printf("Error: %s is not supported yet", _ACTION)
	return
end

-- Check for supported OS
isWindows = os.is( "windows" )
isLinux = os.is( "linux" )
isMacOSX = os.is( "macosx" )
if not isWindows then
	printf("Error: %s is not supported yet", os.get())
	return
end

-- Some Paths
rootDir			= ".."
objectDir		= rootDir .. "/Temp"
libDir			= rootDir .. "/Lib/" .. _ACTION
sourceDir		= rootDir .. "/Source"
thirdPartyDir	= rootDir .. "/ThirdParty"
gameDir			= rootDir .. "/Game"
testDir			= rootDir .. "/Test"
buildDir		= (_ACTION)

-- [start] Settings that are true for all projects
configurations { "Debug", "Release" }
location (buildDir)

filter { "system:windows" }
	defines { "WIN32" }

filter { "action:vs*" }
	defines {"_CRT_SECURE_NO_DEPRECATE" }

filter { "Debug" }
	defines { isVisualStudio and "_DEBUG" or "DEBUG"}
	flags { "Symbols" }

filter { "Release" }
	flags {"ReleaseRuntime"}
	defines { "NDEBUG" }
	--flags { "Optimize" }
	optimize "Speed"

-- [end] Settings that are true for all projects

-- The Game Solution
solution "Game"
	-- Game Project
	project "Game"
		kind "ConsoleApp"
		language "C++"
		location (buildDir)
		objdir( objectDir .. "/Game" )
		defines { "SFML_STATIC", "GLEW_STATIC" }
		includedirs {
			sourceDir,
			thirdPartyDir .. "/Box2D/Box2D",
			thirdPartyDir .. "/TGUI/include",
			thirdPartyDir .. "/SFML/include",
			thirdPartyDir .. "/jsoncpp/include",
			thirdPartyDir .. "/entityx"
		}
		files {
			sourceDir .."/**.h",
			sourceDir .."/**.hpp",
			sourceDir .."/**.cpp"
		}
		--libdirs { thirdPartyDir .. "/SFML/lib/tdm-gcc"}
		libdirs {
			libDir
		}
		if isVisualStudio then 
			libdirs {
				thirdPartyDir .. "/SFML/extlibs/libs-msvc/x86"
			}
		else
			libdirs {
				thirdPartyDir .. "/SFML/extlibs/libs-mingw/x86"
			}
		end
		--linkoptions{"-lTGUI -lBox2D -ljsoncpp -lentityx"}
		filter { "Debug" }
			targetdir( testDir )
			links {
				"TGUI-s-d",
				"Box2D-s-d",
				"jsoncpp-s-d",
				"entityx-s-d",
				"SFML-s-d"
			}
		filter { "Release" }
			targetdir( gameDir )
			links {
				"TGUI-s",
				"Box2D-s",
				"jsoncpp-s",
				"entityx-s",
				"SFML-s"
			}
			
		-- no filter
		filter {}
			links {
				"sndfile",
				"glew",
				"freetype",
				"jpeg",
				"opengl32",
				"gdi32"
			}
			if isWindows then
				links {
					"winmm"
				}
			end
	
-- The Thirdparty Solution
solution "Thirdparty"
	targetdir( libDir )
	location (buildDir)
	
	filter { "Release" }
		targetsuffix  "-s"
	filter { "Debug" }
		targetsuffix  "-s-d"

	-- Thirdparty Projects
	project "entityx"
		kind "StaticLib"
		language "C++"
		objdir( objectDir .. "/entityx" )
		includedirs { thirdPartyDir .. "/entityx" }
		files {
			thirdPartyDir .. "/entityx/entityx/**.h",
			thirdPartyDir .. "/entityx/entityx/**.hpp",
			thirdPartyDir .. "/entityx/entityx/**.cc"
		}
		excludes {
			thirdPartyDir .. "/entityx/**_test.cc"
		}

	project "jsoncpp"
		kind "StaticLib"
		language "C++"
		objdir( objectDir .. "/jsoncpp" )
		includedirs {
			thirdPartyDir .. "/jsoncpp/include"
		}
		files {
			thirdPartyDir .. "/jsoncpp/include/**.h",
			thirdPartyDir .. "/jsoncpp/src/lib_json/**.cpp"
		}

	project "Box2D"
		kind "StaticLib"
		language "C++"
		objdir( objectDir .. "/Box2D" )
		targetdir( libDir )
		location (buildDir)
		includedirs {
			thirdPartyDir .. "/Box2D/Box2D"
		}
		files {
			thirdPartyDir .. "/Box2D/Box2D/Box2D/**.h",
			thirdPartyDir .. "/Box2D/Box2D/Box2D/**.cpp"
		}

	project "TGUI"
		kind "StaticLib"
		language "C++"
		objdir( objectDir .. "/TGUI" )
		targetdir( libDir )
		location (buildDir)
		defines { "SFML_STATIC", "GLEW_STATIC" }
		includedirs {
			thirdPartyDir .. "/TGUI/include",
			thirdPartyDir .. "/SFML/include"
		}
		files {
			thirdPartyDir .. "/TGUI/src/**.h",
			thirdPartyDir .. "/TGUI/src/**.cpp"
		}

	project "SFML"
		kind "StaticLib"
		language "C++"
		objdir( objectDir .. "/SFML" )
		targetdir( libDir )
		location (buildDir)
		defines { "SFML_STATIC", "STBI_FAILURE_USERMSG", "GLEW_STATIC", "UNICODE", "_UNICODE" }
		includedirs {
			thirdPartyDir .. "/SFML/include",
			thirdPartyDir .. "/SFML/src",
			thirdPartyDir .. "/SFML/extlibs/headers",
			thirdPartyDir .. "/SFML/extlibs/headers/AL",
			thirdPartyDir .. "/SFML/extlibs/headers/jpeg"
		}
		files {
			thirdPartyDir .. "/SFML/src/**.h",
			thirdPartyDir .. "/SFML/src/**.cpp"
		}
		if isWindows then
			includedirs {
				thirdPartyDir .. "/SFML/extlibs/headers/libsndfile/windows",
				thirdPartyDir .. "/SFML/extlibs/headers/libfreetype/windows"
			}
			excludes {
				thirdPartyDir .. "/SFML/src/SFML/Network/Unix/**.*",
				thirdPartyDir .. "/SFML/src/SFML/System/Unix/**.*",
				thirdPartyDir .. "/SFML/src/SFML/System/Android/**.*",
				thirdPartyDir .. "/SFML/src/SFML/Window/Android/**.*",
				thirdPartyDir .. "/SFML/src/SFML/Window/FreeBSD/**.*",
				thirdPartyDir .. "/SFML/src/SFML/Window/OSX/**.*",
				thirdPartyDir .. "/SFML/src/SFML/Window/Unix/**.*",
				thirdPartyDir .. "/SFML/src/SFML/Window/EGL*.*",
				thirdPartyDir .. "/SFML/src/SFML/Window/Egl*.*",
				thirdPartyDir .. "/SFML/src/SFML/Main/MainAndroid.cpp",
				thirdPartyDir .. "/SFML/src/SFML/Main/SFMLActivity.cpp"
			}
		end
		if not isVisualStudio then 
			buildoptions  { "-U__STRICT_ANSI__" }
		end
