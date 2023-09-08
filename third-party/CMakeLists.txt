
add_library (glad STATIC "glad/src/glad.c")
if (WIN32)
	target_sources(glad PRIVATE "glad/src/glad_wgl.c")
endif()
target_include_directories(glad PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/glad/include")
