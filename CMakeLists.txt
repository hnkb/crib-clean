cmake_minimum_required(VERSION 3.12)

project(Crib
	VERSION      1.0.0
	DESCRIPTION  TODO
	HOMEPAGE_URL https://github.com/hnkb/crib
)

add_executable(Crib WIN32
	"src/App/Window.cpp"
	"src/Graphics/Font.cpp"
	"src/Graphics/OpenGL/Context_gl.cpp"
	"src/main.cpp"
)

if (WIN32)
	target_sources(Crib PRIVATE
		"src/App/Win/app_win.cpp"
		"src/App/Win/window_win.cpp"
		"src/Graphics/OpenGL/Context_wgl.cpp"
		"src/Platform/Win/COM.cpp"
		"src/Platform/Win/Error_win.cpp"
		"src/Platform/Win/String_win.cpp"
	)

	target_link_libraries(Crib PRIVATE opengl32)
elseif (APPLE)
else()
	# Assuming Unix and Linux here. We can also check (UNIX AND NOT APPLE)

	#target_compile_options(Crib PRIVATE -mavx -Wall -Wextra -pedantic -Werror)

	target_sources(Crib PRIVATE
		"src/App/Xlib/App_x11.cpp"
		"src/App/Xlib/Window_x11.cpp"
		"src/Graphics/OpenGL/Context_glx.cpp"
	)

	find_package(X11 REQUIRED)
	target_link_libraries(Crib PRIVATE X11::X11)
	
	find_package(OpenGL REQUIRED)
	target_link_libraries(Crib PRIVATE OpenGL::OpenGL OpenGL::GLX)
endif()

target_compile_features(Crib PUBLIC cxx_std_17)
target_include_directories(Crib PUBLIC
	"include"
)

if (MSVC)
	target_compile_options(Crib PRIVATE "/utf-8" "/fp:fast" "/Qpar")
	target_link_options(Crib PRIVATE "/ENTRY:mainCRTStartup")
elseif (WIN32 AND (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")) # and CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
	target_link_options(Crib PRIVATE -Xlinker /ENTRY:mainCRTStartup)
endif()


add_subdirectory("third-party")
target_link_libraries(Crib PRIVATE glad ttf2mesh)
