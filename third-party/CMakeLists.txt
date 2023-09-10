
add_library (glad STATIC "glad/src/glad.c")
if (WIN32)
	target_sources(glad PRIVATE "glad/src/glad_wgl.c")
elseif (APPLE)
else()
	target_sources(glad PRIVATE "glad/src/glad_glx.c")
endif()
target_include_directories(glad PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/glad/include")



add_library (ttf2mesh STATIC "ttf2mesh/ttf2mesh.c")
target_include_directories(ttf2mesh PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/ttf2mesh")
