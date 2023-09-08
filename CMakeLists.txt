cmake_minimum_required(VERSION 3.12)

project(crib
	VERSION      1.0.0
	DESCRIPTION  TODO
	HOMEPAGE_URL https://github.com/hnkb/crib
)

add_executable(crib
	"src/app/win/app_win.cpp"
	"src/app/win/com.cpp"
	"src/app/win/error_win.cpp"
	"src/app/win/string_win.cpp"
	"src/app/win/window_win.cpp"
	"src/graphics/gl/context_gl.cpp"
	"src/graphics/gl/gl_win.cpp"
	"src/main.cpp"
)

target_compile_features(crib PUBLIC cxx_std_17)
target_include_directories(crib PUBLIC
	"include"
)

if (MSVC)
	target_compile_options(crib PRIVATE "/utf-8" "/fp:fast" "/Qpar")
endif()

add_subdirectory("third-party")
target_link_libraries(crib PRIVATE glad opengl32)
