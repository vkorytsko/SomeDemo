#pragma once

#include "world.hpp"

#include <memory>


namespace SD::ENGINE {

class NodePropertiesPanel
{
public:
	NodePropertiesPanel() = default;
	~NodePropertiesPanel() = default;

	void Draw(World::Node* node);

private:
	void DrawTransform(World::Node* node);
	void DrawMesh(const World::Mesh* mesh);
	void DrawPrimitive(const uint64_t id, const World::Primitive* primitive);
	void DrawMaterial(const World::Material* material);
};

} // end namespace SD::ENGINE
