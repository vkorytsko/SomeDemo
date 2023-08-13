#include "space.hpp"

#include <iostream>


namespace SD::ENGINE {

Space::Space()
	: m_pTimer(std::make_unique<Timer>())
{
}

void Space::Init()
{
    std::clog << "Space initialization!" << std::endl;

    m_pTimer->GetDelta();

    m_pWorld = std::make_unique<World>("..\\res\\scenes\\Sponza\\glTF\\Sponza.gltf");
    m_pDebugWorld = std::make_unique<DebugWorld>();

    std::clog << "Space loaded: " << m_pTimer->GetDelta() << " s." << std::endl;
}

void Space::Destroy()
{
    std::clog << "Space destroying!" << std::endl;

    m_pTimer->GetDelta();
    m_pWorld.release();

    std::clog << "Space destroyed: " << m_pTimer->GetDelta() << " s." << std::endl;
}

void Space::Simulate(float dt)
{
    m_simulationTime += dt;

    m_pWorld->Simulate(dt);
    m_pDebugWorld->Simulate(dt);
}

void Space::Update(float dt)
{
    m_pWorld->Update(dt);
    m_pDebugWorld->Update(dt);
}

void Space::Draw()
{
    m_pDebugWorld->BindLights();
    m_pWorld->Draw();

    m_pDebugWorld->Draw();
}
}  // end namespace SD::ENGINE
