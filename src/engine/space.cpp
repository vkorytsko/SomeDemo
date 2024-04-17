#include "space.hpp"

#include <iostream>

#include "world.hpp"
#include "debug_world.hpp"

#include "space_settings_panel.hpp"
#include "viewport_panel.hpp"


namespace SD::ENGINE {

Space::Space()
	: m_pTimer(std::make_unique<Timer>())
{
}

Space::~Space()
{
}

void Space::Init()
{
    std::clog << "Space initialization!" << std::endl;

    m_pTimer->GetDelta();

    m_pWorld = std::make_unique<World>(this);
    m_pWorld->Create(SD_RES_DIR + std::string("scenes\\Sponza\\main\\main.gltf"));

    m_pDebugWorld = std::make_unique<DebugWorld>(this);

    m_spaceSettingsPanel = std::make_unique<SpaceSettingsPanel>();
    m_viewportPanel = std::make_unique<ViewportPanel>();

    std::clog << "Space loaded: " << m_pTimer->GetDelta() << " s." << std::endl;
}

void Space::Destroy()
{
    std::clog << "Space destroying!" << std::endl;

    m_pTimer->GetDelta();

    m_pWorld.reset();
    m_pDebugWorld.reset();

    std::clog << "Space destroyed: " << m_pTimer->GetDelta() << " s." << std::endl;
}

void Space::Simulate(float dt)
{
    if (m_simulationPaused)
    {
        return;
    }

    dt *= m_simulationFactor;
    m_simulationTime += dt;

    m_pWorld->Simulate(dt);
    m_pDebugWorld->Simulate(dt);
}

void Space::Update(float dt)
{
    m_pWorld->Update(dt);
    m_pDebugWorld->Update(dt);
}

void Space::DrawFrame()
{
    m_pDebugWorld->BindLights();
    m_pWorld->Draw();

    m_pDebugWorld->Draw();
}
void Space::DrawImGui()
{
    m_pWorld->DrawImGui();
    m_pDebugWorld->DrawImGui();

    m_spaceSettingsPanel->Draw(this);
    m_viewportPanel->Draw();
}
}  // end namespace SD::ENGINE
