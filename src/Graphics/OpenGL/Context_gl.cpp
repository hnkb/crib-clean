
#include "Context.h"
#include <glad/glad.h>
#include <algorithm>
#include <vector>

#include "../Font.h"

using Crib::Graphics::OpenGL::Context;


GLuint theProgram = 0;


GLuint CreateShader(GLenum eShaderType, const std::string& strShaderFile)
{
	GLuint shader = glCreateShader(eShaderType);
	const char* strFileData = strShaderFile.c_str();
	glShaderSource(shader, 1, &strFileData, NULL);

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		const char* strShaderType = NULL;
		switch (eShaderType)
		{
			case GL_VERTEX_SHADER:
				strShaderType = "vertex";
				break;
			case GL_GEOMETRY_SHADER:
				strShaderType = "geometry";
				break;
			case GL_FRAGMENT_SHADER:
				strShaderType = "fragment";
				break;
		}

		fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
		delete[] strInfoLog;
	}

	return shader;
}

GLuint CreateProgram(const std::vector<GLuint>& shaderList)
{
	GLuint program = glCreateProgram();

	for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glAttachShader(program, shaderList[iLoop]);

	glLinkProgram(program);

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}

	for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glDetachShader(program, shaderList[iLoop]);

	return program;
}



const std::string strVertexShader(
	"#version 330 core\n"
	"layout(location = 0) in vec4 position;"
	"void main()"
	"{"
	"   gl_Position = position;"
	"}");

const std::string strFragmentShader(
	"#version 330 core\n"
	"out vec4 outputColor;"
	"void main()"
	"{"
	"   outputColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);"
	"}");

void InitializeProgram()
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(CreateShader(GL_VERTEX_SHADER, strVertexShader));
	shaderList.push_back(CreateShader(GL_FRAGMENT_SHADER, strFragmentShader));

	theProgram = CreateProgram(shaderList);

	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
}

const float4 vertexPositions[] = {
	{ 0.75f,  0.75f, 0.0f, 1.0f},
	{ 0.75f, -0.75f, 0.0f, 1.0f},
	{-0.75f, -0.75f, 0.0f, 1.0f},
};

GLuint positionBufferObject;
GLuint vao;

float scaleX = 1.f;
std::vector<float4> fontVertices;

void InitializeVertexBuffer()
{
	Font font(LR"(C:\Users\hani\Downloads\overpass-bold.ttf)");

	float scaleFactor = 1.1f*.3f;
	float2 scale = { scaleFactor / scaleX, scaleFactor };
	float2 xlate = { -.8f, -.5f };

	for (auto symbol : std::wstring(L"crib graphics playground"))
	{
		if (symbol == ' ')
			xlate.x += .15f * scale.x;
		try
		{
			auto g = font.getGlyph(symbol);

			//printf("x bounds %f %f\n", g.glyph->xbounds[0], g.glyph->xbounds[1]);
			//printf("y bounds %f %f\n", g.glyph->ybounds[0], g.glyph->ybounds[1]);
			//printf("Faces: %d, Vertices: %d\n", g.mesh->nfaces, g.mesh->nvert);

			fontVertices.reserve(g.mesh->nfaces * 3);

			for (int i = 0; i < g.mesh->nfaces; i++)
			{
				auto getVert = [&](auto idx) {
					auto& v = g.mesh->vert[idx];
					return float4 { (v.x /*+ g.glyph->lbearing*/) * scale.x + xlate.x,
									v.y * scale.y + xlate.y,
									0.f,
									1.f };
				};

				auto& a = getVert(g.mesh->faces[i].v1);
				auto& b = getVert(g.mesh->faces[i].v2);
				auto& c = getVert(g.mesh->faces[i].v3);

				// printf("   %2d -> %f %f    %f %f    %f %f\n", i, a.x, a.y, b.x, b.y, c.x,
				// c.y);

				fontVertices.push_back(a);
				fontVertices.push_back(b);
				fontVertices.push_back(c);
			}

			xlate.x +=
				(g.glyph->xbounds[1] - g.glyph->xbounds[0] + g.glyph->rbearing) * scale.x;
			// xlate.x += g.glyph->advance * scale.x;
		}
		catch (...)
		{
			xlate.x += .05f * scale.x;
		}
	}

	glGenBuffers(1, &positionBufferObject);

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(float4) * fontVertices.size(),
		fontVertices.data(),
		GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void initGL_3()
{
	InitializeProgram();
	InitializeVertexBuffer();

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
}


void Context::onResize(int2 dims)
{
	glViewport(0, 0, dims.x, dims.y);

	scaleX = dims.x / (float)dims.y;

	//float w = dims.x;
	//float h = dims.y;
	//if (h > w)
	//	glViewport(0, (h - w) / 2, w, w);
	//else
	//	glViewport((w - h) / 2, 0, h, h);
}

void Context::readDeviceDescription(int swapInterval)
{
	std::string profile;
	{
		GLint mask;
		glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &mask);
		if (mask & GL_CONTEXT_CORE_PROFILE_BIT)
			profile = "Core";
		else if (mask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT)
			profile = "Compatibility";
	}

	std::string sync;
	{
		if (swapInterval == 1)
			sync = " (V-Sync)";
		else if (swapInterval == -1)
			sync = " (adaptive sync)";
	}

	description = std::string("GL ") + (char*)glGetString(GL_VERSION) + "  " + profile
				  + "  GLSL " + (char*)glGetString(GL_SHADING_LANGUAGE_VERSION) + "  |  "
				  + (char*)glGetString(GL_RENDERER) + sync;
}

void Context::drawPlatformIndependent()
{
	glClearColor(0.6f, 0.2f, 0.15f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (theProgram == 0)
		initGL_3();

	glUseProgram(theProgram);

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferObject);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawArrays(GL_TRIANGLES, 0, fontVertices.size());

	glDisableVertexAttribArray(0);
	glUseProgram(0);
}
