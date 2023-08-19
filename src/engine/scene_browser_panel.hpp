#pragma once

#include "world.hpp"

#include <memory>


namespace SD::ENGINE {

class SceneBrowserPanel
{
public:
	SceneBrowserPanel() = default;
	~SceneBrowserPanel() = default;

	void Draw(World* world);

	World::Node* selectedNode() const { return m_selectedNode; }

private:
	void DrawScenesOverview(World* world);
	void DrawHierarchy(const World::Scene* scene);
	void DrawNode(World::Node* node);

	World::Node* m_selectedNode = nullptr;
};

} // end namespace SD::ENGINE
