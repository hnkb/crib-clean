cmake_minimum_required(VERSION 3.12)

project(crib
	VERSION      1.0.0
	DESCRIPTION  TODO
	HOMEPAGE_URL https://github.com/hnkb/crib
)

add_executable(crib
	"src/App/Window.cpp"
	"src/Graphics/OpenGL/Context_gl.cpp"
	"src/main.cpp"
)

if (WIN32)
	target_sources(crib PRIVATE
		"src/App/Win/app_win.cpp"
		"src/App/Win/window_win.cpp"
		"src/Graphics/OpenGL/Context_wgl.cpp"
		"src/Platform/Win/COM.cpp"
		"src/Platform/Win/Error_win.cpp"
		"src/Platform/Win/String_win.cpp"
	)

	target_link_libraries(crib PRIVATE opengl32)
elseif (APPLE)
else()
	# Assuming Unix and Linux here. We can also check (UNIX AND NOT APPLE)

	#target_compile_options(crib PRIVATE -mavx -Wall -Wextra -pedantic -Werror)

	target_sources(crib PRIVATE
		"src/App/Xlib/App_x11.cpp"
		"src/App/Xlib/Window_x11.cpp"
		"src/Graphics/OpenGL/Context_glx.cpp"
	)

	find_package(X11 REQUIRED)
	target_link_libraries(crib PRIVATE X11::X11)
	
	find_package(OpenGL REQUIRED)
	target_link_libraries(crib PRIVATE OpenGL::OpenGL OpenGL::GLX)
endif()

target_compile_features(crib PUBLIC cxx_std_17)
target_include_directories(crib PUBLIC
	"include"
)

if (MSVC)
	target_compile_options(crib PRIVATE "/utf-8" "/fp:fast" "/Qpar")
endif()

add_subdirectory("third-party")
target_link_libraries(crib PRIVATE glad)
