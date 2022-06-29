#include "application.hpp"

#include <iostream>
#include <stdint.h>


const wchar_t* NAME = L"Some Demo";
const uint32_t WIDTH = 1200;
const uint32_t HEIGHT = 800;


Application::Application()
{
	std::clog << "Application initialization!" << std::endl;

	m_pWindow = std::make_unique<Window>(WIDTH, HEIGHT, NAME);
	m_pRenderer = std::make_unique<Renderer>(m_pWindow->GetHandle());
}

Application::~Application()
{
	std::clog << "Application destroying!" << std::endl;
}

int Application::Run()
{
	while (true) {
		if (const auto exitcode = m_pWindow->ProcessMessages()) {
			return *exitcode;
		}

		m_pRenderer->Render();
	}
}
