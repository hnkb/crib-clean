
#include "Font.h"
#include <stdexcept>

using Crib::Font;

Font::Font(const std::filesystem::path& filename)
{
	ttf_load_from_file(filename.u8string().c_str(), &font, false);
	if (!font)
		throw std::runtime_error("Unable to load font");
}

Font::Glyph::~Glyph()
{
	if (mesh)
		ttf_free_mesh(mesh);
}

Font::Glyph Font::getGlyph(wchar_t symbol)
{
	auto index = ttf_find_glyph(font, symbol);
	if (index < 0)
		throw std::invalid_argument("glyph not found in the font");
	return getGlyph(index);
}

Font::Glyph Font::getGlyph(int index)
{
	Glyph ret;
	ret.glyph = &font->glyphs[index];

	if (ttf_glyph2mesh(&font->glyphs[index], &ret.mesh, TTF_QUALITY_HIGH, TTF_FEATURES_DFLT)
		!= TTF_DONE)
		throw std::runtime_error("unable to load glyph mesh");

	return ret;
}

#if (0)
	static std::vector<ttf_t*> getSystemFonts()
	{
		auto list = ttf_list_system_fonts(nullptr);

		if (!list || !list[0])
			throw std::runtime_error("No system font found");

		auto item = list[0];
		while (item)
		{
			printf(" - %s\n", item->filename);
			item++;
		}
		// ttf_t** list = ttf_list_system_fonts("DejaVuSans*|Ubuntu*|FreeSerif*|Arial*|Cour*");
		// ttf_free_list(list);
	}
#endif

#if 0
static void on_render()
{
	if (mesh != NULL)
	{
		glTranslatef(width / 2, height / 10, 0);
		glScalef(0.9f * height, 0.9f * height, 1.0f);
		glScalef(1.0f, 1.0f, 0.1f);
		glTranslatef(-(glyph->xbounds[0] + glyph->xbounds[1]) / 2, -glyph->ybounds[0], 0.0f);

		/* Draw wireframe/solid */
		if (view_mode == 0 || view_mode == 1)
		{
			glColor3f(0.0, 0.0, 0.0);
			glPolygonMode(GL_FRONT_AND_BACK, view_mode == 1 ? GL_LINE : GL_FILL);
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(2, GL_FLOAT, 0, &mesh->vert->x);
			glDrawElements(GL_TRIANGLES, mesh->nfaces * 3, GL_UNSIGNED_INT, &mesh->faces->v1);
			glDisableClientState(GL_VERTEX_ARRAY);
		}

		/* Draw contours */
		if (view_mode == 2)
		{
			int i;
			for (i = 0; i < mesh->outline->ncontours; i++)
			{
				glColor3f(0.0, 0.0, 0.0);
				glLineWidth(1.0);
				glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer(
					2,
					GL_FLOAT,
					sizeof(ttf_point_t),
					&mesh->outline->cont[i].pt->x);
				glDrawArrays(GL_LINE_LOOP, 0, mesh->outline->cont[i].length);
				glDisableClientState(GL_VERTEX_ARRAY);
			}
		}
	}
}
#endif


void startFont()
{
	Font font(LR"(C:\Users\hani\Downloads\overpass-bold.ttf)");
	printf("'%s' font loaded.\n", font->names.full_name);

	printf("  number of glyphs: %d\n", font->nglyphs);
	printf("  number of chars:  %d\n", font->nchars);

	printf("\n\n\nGlyphs:\n");
	for (int i = 0; i < font->nglyphs; i++)
			printf("%c", font->glyphs[i].symbol);
	printf("\n\n\nChars:\n");
	for (int i = 0; i < font->nchars; i++)
			printf("%c", font->chars[i]);
	return;
	auto g = font.getGlyph('g');

	printf("x bounds %f %f\n", g.glyph->xbounds[0], g.glyph->xbounds[1]);
	printf("y bounds %f %f\n", g.glyph->ybounds[0], g.glyph->ybounds[1]);
	printf("Faces: %d, Vertices: %d\n", g.mesh->nfaces, g.mesh->nvert);

	for (int i = 0; i < g.mesh->nfaces; i++)
	{
		auto& a = g.mesh->vert[g.mesh->faces[i].v1];
		auto& b = g.mesh->vert[g.mesh->faces[i].v2];
		auto& c = g.mesh->vert[g.mesh->faces[i].v3];

		printf("   %2d -> %f %f    %f %f    %f %f\n", i, a.x, a.y, b.x, b.y, c.x, c.y);
	}
}
