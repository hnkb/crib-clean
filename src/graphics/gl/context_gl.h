
#pragma once

#include <crib/graphics>
#include <crib/app>

#if defined(_WIN32)
#	include <crib/platform/win>
#endif


namespace crib::graphics::gl
{

	class context : public graphics::context
	{
	public:
		context(const app::window&);
		virtual ~context();

		virtual void draw() override;
		virtual void on_resize(int2 dims) override;

	private:
		HGLRC ctx = nullptr;
		const app::window& owner;
	};

}
