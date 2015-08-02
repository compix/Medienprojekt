require('netbeans')

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
if not isWindows and not isLinux then
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
if isLinux and isNetbeans then
	location ("netbeans-linux")
else
	location (buildDir)
end

flags {"C++11", "nbProjectFolder"}

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
		objdir( objectDir .. "/Game" )
		defines { "SFML_STATIC", "GLEW_STATIC", "USING_SIGNAL11", "USING_ECSTASY" }
		includedirs {
			thirdPartyDir .. "/Box2D/Box2D",
			thirdPartyDir .. "/TGUI/include",
			thirdPartyDir .. "/SFML/include",
			thirdPartyDir .. "/enet/include",
			thirdPartyDir .. "/jsoncpp/include",
			thirdPartyDir .. "/cppformat",
			thirdPartyDir .. "/ecstasy/include"
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
			defines { "HAS_STD_MAKE_UNIQUE" }
			libdirs {
				thirdPartyDir .. "/SFML/extlibs/libs-msvc/x86"
			}
		elseif isWindows then
			libdirs {
				thirdPartyDir .. "/SFML/extlibs/libs-mingw/x86"
			}
		end
		--linkoptions{"-lTGUI -lBox2D -ljsoncpp -lecstasy"}
		filter { "Debug" }
			targetdir( testDir )
			links {
				"TGUI-s-d",
				"Box2D-s-d",
				"enet-s-d",
				"jsoncpp-s-d",
				"ecstasy-s-d",
				"cppformat-s-d",
				"SFML-s-d"
			}
		filter { "Release" }
			targetdir( gameDir )
			links {
				"TGUI-s",
				"Box2D-s",
				"enet-s",
				"jsoncpp-s",
				"ecstasy-s",
				"cppformat-s",
				"SFML-s"
			}
			
		-- no filter
		filter {}
			links {
				"openal32",
				"sndfile",
				"GLEW",
				"freetype",
				"jpeg"
			}
			if isWindows then
				links {
					"winmm",
					"ws2_32",
					"gdi32",
					"opengl32"
				}
			else
				links {
					"GL",
					"X11",
					"Xrandr",
					"pthread",
					"udev"
				}
			end
	
-- The Thirdparty Solution
solution "Thirdparty"
	targetdir( libDir )
	
	filter { "Release" }
		targetsuffix  "-s"
	filter { "Debug" }
		targetsuffix  "-s-d"

	-- Thirdparty Projects
	project "enet"
		kind "StaticLib"
		language "C"
		objdir( objectDir .. "/enet" )
		includedirs { thirdPartyDir .. "/enet/include" }
		files {
			thirdPartyDir .. "/enet/**.h",
			thirdPartyDir .. "/enet/**.c"
		}
		if isWindows then 
			excludes {
				thirdPartyDir .. "/enet/unix.c"
			}
		else
			defines { "HAS_SOCKLEN_T" }
			excludes {
				thirdPartyDir .. "/enet/win32.c"
			}
		end

	project "cppformat"
		kind "StaticLib"
		language "C++"
		objdir( objectDir .. "/cppformat" )
		includedirs { thirdPartyDir .. "/cppformat" }
		files {
			thirdPartyDir .. "/cppformat/format.h",
			thirdPartyDir .. "/cppformat/format.cc"
		}
		
	project "ecstasy"
		kind "StaticLib"
		language "C++"
		objdir( objectDir .. "/ecstasy" )
		includedirs {
			thirdPartyDir .. "/ecstasy/include"
		}
		files {
			thirdPartyDir .. "/ecstasy/include/**.h",
			thirdPartyDir .. "/ecstasy/source/**.h",
			thirdPartyDir .. "/ecstasy/source/**.cpp"
		}
		targetdir( libDir )
		filter { "Release" }
			targetsuffix  "-s"
		filter { "Debug" }
			targetsuffix  "-s-d"

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
		elseif isLinux then
			includedirs {
				"/usr/include/freetype2"
			}
			excludes {
				thirdPartyDir .. "/SFML/src/SFML/Network/Win32/**.*",
				thirdPartyDir .. "/SFML/src/SFML/System/Win32/**.*",
				thirdPartyDir .. "/SFML/src/SFML/System/Android/**.*",
				thirdPartyDir .. "/SFML/src/SFML/Window/Android/**.*",
				thirdPartyDir .. "/SFML/src/SFML/Window/FreeBSD/**.*",
				thirdPartyDir .. "/SFML/src/SFML/Window/OSX/**.*",
				thirdPartyDir .. "/SFML/src/SFML/Window/Win32/**.*",
				thirdPartyDir .. "/SFML/src/SFML/Window/EGL*.*",
				thirdPartyDir .. "/SFML/src/SFML/Window/Egl*.*",
				thirdPartyDir .. "/SFML/src/SFML/Main/MainAndroid.cpp",
				thirdPartyDir .. "/SFML/src/SFML/Main/SFMLActivity.cpp"
			}
		end
		if not isVisualStudio then 
			buildoptions  { "-U__STRICT_ANSI__" }
		end
