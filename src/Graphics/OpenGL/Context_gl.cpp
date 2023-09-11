
#include "Context.h"
#include <glad/glad.h>
#include <algorithm>
#include <vector>
#include <map>

#include "../Font.h"

using Crib::Graphics::OpenGL::Context;

extern float2 offset;
extern float scale;
extern std::string text;
extern float scaleX;

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
	"layout(location = 0) in vec2 position;"
	"uniform vec2 offset;"
	"uniform vec2 scale;"
	"void main()"
	"{"
	"   gl_Position = vec4((position+offset)*scale, 0, 1.0);"
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



class Buffer
{
public:
	GLuint vertex;
	GLuint index;
} buffer;


struct DrawableObject
{
	void* start;
	GLsizei numPoints;
	GLuint objectId;

	float advance;
	float lbearing;
};
std::map<wchar_t, DrawableObject> objects;


DrawableObject tt;

void InitializeVertexBuffer()
{
	Crib::Font font("/home/hani/Desktop/overpass-bold.ttf");


	// temporary buffers to store CPU-side data

	std::vector<float2> vert;
	std::vector<GLushort> index;
	std::map<wchar_t, void*> vertOffsets;

	vert.reserve(font->nglyphs * 125);
	index.reserve(font->nglyphs * 125);


	int numErrors = 0;
	for (int glyphIdx = 0; glyphIdx < font->nglyphs; glyphIdx++)
	{
		try
		{
			auto g = font.getGlyph(glyphIdx);

			//wprintf(L" '%c' %d vert  %d idx\n", g.glyph->symbol, g.mesh->nvert, g.mesh->nfaces);


			auto& obj = objects[g.glyph->symbol];

			obj.advance = g.glyph->advance;
			obj.lbearing = g.glyph->lbearing;
			obj.start = (void*)(index.size() * sizeof(GLushort));
			vertOffsets[g.glyph->symbol] = (void*)(vert.size() * sizeof(vert[0]));

			for (int i = 0; i < g.mesh->nvert; i++)
				vert.push_back(float2 { g.mesh->vert[i].x, g.mesh->vert[i].y });

			for (int i = 0; i < g.mesh->nfaces; i++)
			{
				index.push_back(g.mesh->faces[i].v1);
				index.push_back(g.mesh->faces[i].v2);
				index.push_back(g.mesh->faces[i].v3);
			}

			obj.numPoints = g.mesh->nfaces * 3;
		}
		catch (...)
		{
			numErrors++;
		}
	}

	printf("Encountered %d errors while loading\n", numErrors);
	// printf("\n\n In total\n   %d vertices\n   %d indices\n", vert.size(), index.size());


	//////////////////// create buffers and copy data //////////////////////////

	glGenBuffers(1, &buffer.vertex);
	glBindBuffer(GL_ARRAY_BUFFER, buffer.vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert[0]) * vert.size(), vert.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &buffer.index);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.index);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		sizeof(index[0]) * index.size(),
		index.data(),
		GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	////////////////////// create objects for each glyph ///////////////////////

	for (auto& obj : objects)
	{
		glGenVertexArrays(1, &obj.second.objectId);
		glBindVertexArray(obj.second.objectId);

		glBindBuffer(GL_ARRAY_BUFFER, buffer.vertex);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, vertOffsets.at(obj.first));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.index);

		glBindVertexArray(0);
	}


	{
		glGenVertexArrays(1, &tt.objectId);
		glBindVertexArray(tt.objectId);

		glBindBuffer(GL_ARRAY_BUFFER, buffer.vertex);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.index);

		glBindVertexArray(0);
	}
}

GLint offsetLocation;
GLint scaleLocation;

void initGL_3()
{
	InitializeProgram();
	InitializeVertexBuffer();

	offsetLocation = glGetUniformLocation(theProgram, "offset");
	scaleLocation = glGetUniformLocation(theProgram, "scale");
}


void Context::onResize(int2 dims)
{
	glViewport(0, 0, dims.x, dims.y);

	scaleX = dims.x / (float)dims.y;
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

	float2 scale2d = { scale / scaleX, scale };
	//glUniform1f(scaleLocation, scale);
	glUniform2fv(scaleLocation, 1, (float*)&scale2d);

	float xStart = -2;
	//-.8 / scale;
	float2 textPos = { xStart, -xStart - .9f };

	for (auto symbol : text + "_")  // std::wstring(L"prognosis"))
	{
		if (symbol == ' ')
			textPos.x += .2f;
		else if (symbol == '\n'||symbol=='\r')
		{
			textPos.x = xStart;
			textPos.y -= 1.1f;
		}
		else if (objects.find(symbol) != objects.end())
		{
			auto& obj = objects.at(symbol);
			auto pos = offset + textPos;
			//pos.x += obj.lbearing * scale;
			glBindVertexArray(obj.objectId);
			glUniform2fv(offsetLocation, 1, (float*)&pos);
			glDrawElements(GL_TRIANGLES, obj.numPoints, GL_UNSIGNED_SHORT, obj.start);
			textPos.x += obj.advance;
		}
	}

	glBindVertexArray(0);
	glUseProgram(0);
}
