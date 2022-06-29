#pragma once

#include <memory>

#include "renderer.hpp"
#include "window.hpp"


class Application
{
public:
	Application();
	~Application();

	int Run();

private:
	std::unique_ptr<Window> m_pWindow;
	std::unique_ptr<Renderer> m_pRenderer;
};
