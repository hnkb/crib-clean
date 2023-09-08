
#include "context_gl.h"
#include <glad/glad.h>

using crib::graphics::gl::context;


void context::on_resize(int2 dims)
{
	glViewport(0, 0, dims.x, dims.y);
}
