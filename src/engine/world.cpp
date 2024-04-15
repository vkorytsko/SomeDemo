#define NOMINMAX

#include "world.hpp"

#include <filesystem>
#include <iostream>
#include <unordered_map>

#include "application.hpp"
#include "utils.hpp"

#include "scene_browser_panel.hpp"
#include "node_properties_panel.hpp"
#include "space_settings_panel.hpp"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE 
#define TINYGLTF_NO_STB_IMAGE_WRITE 
#define TINYGLTF_NO_EXTERNAL_IMAGE  
#define TINYGLTF_USE_CPP14
#include "tiny_gltf.h"


namespace
{
typedef std::pair<int, int> BufferFormat;

struct buffer_format_hasher {
	int64_t operator() (const BufferFormat& p) const {
		return ((static_cast<int64_t>(p.first)) << 32) | static_cast<int64_t>(p.second);
	}
};

const std::unordered_map<const BufferFormat, DXGI_FORMAT, buffer_format_hasher> BUFFER_FORMATS = {
	{{TINYGLTF_COMPONENT_TYPE_FLOAT, TINYGLTF_TYPE_VEC2}, DXGI_FORMAT_R32G32_FLOAT},
	{{TINYGLTF_COMPONENT_TYPE_FLOAT, TINYGLTF_TYPE_VEC3}, DXGI_FORMAT_R32G32B32_FLOAT},
	{{TINYGLTF_COMPONENT_TYPE_FLOAT, TINYGLTF_TYPE_VEC4}, DXGI_FORMAT_R32G32B32A32_FLOAT},
};
}

namespace SD::ENGINE {

World::World(const Space* space)
	: m_pTimer(std::make_unique<Timer>())
	, m_space(space)
{
}

World::~World()
{
}

void World::Create(const std::string& path, const DirectX::XMMATRIX& transform)
{
	m_transform = transform;

	const auto& model = load(path);

	createBuffers(model);
	createTextures(model, std::filesystem::path(path).remove_filename());
	createSamplers(model);
	createMaterials(model);
	createMeshes(model);
	createNodes(model);
	createScenes(model);

	m_selectedScene = model.defaultScene;

	m_sceneBrowserPanel = std::make_unique<SceneBrowserPanel>();
	m_nodePropertiesPanel = std::make_unique<NodePropertiesPanel>();
}

void World::Simulate(float dt)
{
	for (auto& scene : m_scenes)
	{
		scene->Simulate(dt);
	}
}

void World::Update(float dt)
{
	for (auto& scene : m_scenes)
	{
		scene->Update(dt);
	}
}

void World::Draw()
{
	m_scenes[m_selectedScene]->Draw();
}

void World::DrawImGui()
{
	m_sceneBrowserPanel->Draw(this);

	auto* selectedNode = m_sceneBrowserPanel->selectedNode();
	m_nodePropertiesPanel->Draw(selectedNode);
}

tinygltf::Model World::load(const std::string& path) const
{
	tinygltf::Model model;
	tinygltf::TinyGLTF loader;
	std::string error;
	std::string warning;

	const bool res = loader.LoadASCIIFromFile(&model, &error, &warning, path);

	if (!warning.empty())
	{
		std::clog << "TinyGLTF Warning: " << warning << std::endl;
	}

	if (!error.empty())
	{
		std::clog << "TinyGLTF Error: " << error << std::endl;
	}

	if (!res)
	{
		std::clog << "Failed to parse glTF" << std::endl;
	}

	return model;
}

void World::createBuffers(const tinygltf::Model& model)
{
	std::clog << "Create buffers!" << std::endl;

	const auto& app = Application::GetApplication();
	const auto& renderer = app->GetRenderer();

	m_buffers.reserve(model.bufferViews.size());

	for (const auto& bufferView : model.bufferViews)
	{
		if (bufferView.target == TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER)
		{
			m_buffers.emplace_back(std::make_unique<RENDER::IndexBuffer>());
		}
		else
		{
			m_buffers.emplace_back(std::make_unique<RENDER::VertexBuffer>());
		}

		const auto buffer = model.buffers[bufferView.buffer];
		m_buffers.back()->create(renderer, buffer.data.data() + bufferView.byteOffset, bufferView.byteLength);
	}

	std::clog << "Buffers created: " << m_pTimer->GetDelta() << " s." << std::endl;
}

void World::createTextures(const tinygltf::Model& model, const std::filesystem::path& dir)
{
	std::clog << "Create textures!" << std::endl;

	const auto& app = Application::GetApplication();
	const auto& renderer = app->GetRenderer();

	for (const auto& image : model.images)
	{
		const auto path = dir.string() + image.uri;
		m_textures.emplace_back(std::make_shared<RENDER::Texture>(renderer, AToWstring(path)));
	}

	std::clog << "Textures created: " << m_pTimer->GetDelta() << " s." << std::endl;
}

void World::createSamplers(const tinygltf::Model& model)
{
	std::clog << "Create samplers!" << std::endl;

	const auto& app = Application::GetApplication();
	const auto& renderer = app->GetRenderer();

	for (const auto& sampler : model.samplers)
	{
		m_samplers.emplace_back(std::make_shared<RENDER::Sampler>(renderer));
	}

	std::clog << "Samplers created: " << m_pTimer->GetDelta() << " s." << std::endl;
}

void World::createMaterials(const tinygltf::Model& model)
{
	std::clog << "Create materials!" << std::endl;

	for (const auto& material : model.materials)
	{
		const auto id = static_cast<uint32_t>(m_materials.size());
		const std::string name = material.name.empty() ? "Material " + std::to_string(id) : material.name;
		m_materials.emplace_back(std::make_shared<Material>(name, id))->Setup(this, model, material);
	}

	std::clog << "Materials created: " << m_pTimer->GetDelta() << " s." << std::endl;
}

void World::createMeshes(const tinygltf::Model& model)
{
	std::clog << "Create meshes!" << std::endl;

	for (const auto& mesh : model.meshes)
	{
		const auto id = static_cast<uint32_t>(m_meshes.size());
		const std::string name = mesh.name.empty() ? "Mesh " + std::to_string(id) : mesh.name;
		m_meshes.emplace_back(std::make_shared<Mesh>(name, id))->Setup(this, model, mesh);
	}

	std::clog << "Meshes created: " << m_pTimer->GetDelta() << " s." << std::endl;
}

void World::createNodes(const tinygltf::Model& model)
{
	std::clog << "Create nodes!" << std::endl;

	for (const auto& node : model.nodes)
	{
		const auto id = static_cast<uint32_t>(m_nodes.size());
		const std::string name = node.name.empty() ? "Node " + std::to_string(id) : node.name;
		m_nodes.emplace_back(std::make_shared<Node>(name, id))->Setup(this, node);
	}

	std::clog << "Nodes created: " << m_pTimer->GetDelta() << " s." << std::endl;
}

void World::createScenes(const tinygltf::Model& model)
{
	std::clog << "Create scenes!" << std::endl;

	for (const auto& scene : model.scenes)
	{
		const auto id = static_cast<uint32_t>(m_scenes.size());
		const std::string name = scene.name.empty() ? "Scene " + std::to_string(id) : scene.name;
		m_scenes.emplace_back(std::make_shared<Scene>(name, id))->Setup(this, model, scene);
	}

	std::clog << "Scenes created: " << m_pTimer->GetDelta() << " s." << std::endl;
}

World::Scene::Scene(const std::string& name, const uint32_t id)
	: m_name(name)
	, m_id(id)
{
}

void World::Scene::Setup(const World* world, const tinygltf::Model& model, const tinygltf::Scene& scene)
{
	constexpr auto id = std::numeric_limits<uint32_t>::max();
	const std::string name = "root";
	m_root = std::make_shared<Node>(name, id, world->m_transform);
	m_root->m_children = getChildren(world, scene.nodes);

	for (const auto nodeIdx : scene.nodes)
	{
		buildHierarchy(world, model, m_root, nodeIdx);
	}
}

void World::Scene::buildHierarchy(
	const World* world,
	const tinygltf::Model& model,
	const std::shared_ptr<Node>& parent,
	const int nodeIdx) const
{
	const auto& node = world->m_nodes[nodeIdx];
	const auto& children = model.nodes[nodeIdx].children;

	node->m_parent = parent;
	node->m_children = getChildren(world, children);

	for (const auto childIdx : children)
	{
		buildHierarchy(world, model, node, childIdx);
	}
}

const std::vector<std::shared_ptr<World::Node>> World::Scene::getChildren(
	const World* world,
	const std::vector<int>& children) const
{
	std::vector<std::shared_ptr<Node>> result;
	result.reserve(children.size());
	for (const auto childIdx : children)
	{
		result.emplace_back(world->m_nodes[childIdx]);
	}
	
	return result;
}

void World::Scene::Simulate(float dt)
{
	m_root->Simulate(dt);
}

void World::Scene::Update(float dt)
{
	m_root->Update(dt);
}

void World::Scene::Draw()
{
	m_root->Draw();
}

World::Node::Node(const std::string& name, const uint32_t id, const DirectX::XMMATRIX& transform)
	: m_name(name)
	, m_id(id)
	, m_localTransform(transform)
{
	DirectX::XMMatrixDecompose(&m_originalScale, &m_originalRotation, &m_originalTranslation, m_localTransform);
}

#pragma warning( push )
#pragma warning( disable : 4244 )
void World::Node::Setup(const World* world, const tinygltf::Node& node)
{
	if (node.mesh >= 0)
	{
		m_mesh = world->m_meshes[node.mesh];
	}

	if (!node.matrix.empty())
	{
		const std::vector<float> matrix(node.matrix.begin(), node.matrix.end());
		m_localTransform = DirectX::XMMATRIX(matrix.data());
	}

	if (!node.scale.empty())
	{
		const std::vector<float> tmp(node.scale.begin(), node.scale.end());
		const auto scale = DirectX::XMFLOAT3(tmp.data());
		m_localTransform *= DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&scale));
	}

	if (!node.rotation.empty())
	{
		const std::vector<float> tmp(node.rotation.begin(), node.rotation.end());
		const auto rotation = DirectX::XMFLOAT4(tmp.data());
		m_localTransform *= DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&rotation));
	}

	if (!node.translation.empty())
	{
		const std::vector<float> tmp(node.translation.begin(), node.translation.end());
		const auto translation = DirectX::XMFLOAT3(tmp.data());
		m_localTransform *= DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&translation));
	}

	DirectX::XMMatrixDecompose(&m_originalScale, &m_originalRotation, &m_originalTranslation, m_localTransform);

	const auto& app = Application::GetApplication();
	const auto& renderer = app->GetRenderer();

	// create constant buffers
	CB_transform transformCB;
	m_pTransformCB = std::make_unique<SD::RENDER::ConstantBuffer<CB_transform>>(renderer, transformCB);
}
#pragma warning( pop )

void World::Node::Simulate(float dt)
{
	// update transform
	if (const auto parent = m_parent.lock())
	{
		m_worldTransform = m_localTransform * parent->m_worldTransform;
	}
	else
	{
		m_worldTransform = m_localTransform;
	}

	for (const auto& child : m_children)
	{
		child->Simulate(dt);
	}
}

void World::Node::Update(float dt)
{
	const auto& app = Application::GetApplication();
	const auto& camera = app->GetCamera();
	const auto& renderer = app->GetRenderer();

	if (m_mesh)
	{
		// update transform constant buffer
		{
			const auto& transformCB = m_pTransformCB->GetData();
			transformCB->model = m_worldTransform;
			transformCB->view = camera->getView();
			transformCB->projection = camera->getProjection();
			transformCB->viewPosition = camera->getPosition();
			m_pTransformCB->Update(renderer);
		}
	}

	for (const auto& child : m_children)
	{
		child->Update(dt);
	}
}

void World::Node::Draw()
{
	const auto& app = Application::GetApplication();
	const auto& renderer = app->GetRenderer();

	if (m_mesh)
	{
		m_pTransformCB->VSBind(renderer, 0u);

		m_mesh->Draw();
	}

	for (const auto& child : m_children)
	{
		child->Draw();
	}
}

World::Material::Material(const std::string& name, const uint32_t id)
	: m_name(name)
	, m_id(id)
{
}

void World::Material::Setup(const World* world, const tinygltf::Model& model, const tinygltf::Material& material)
{
	const auto& app = Application::GetApplication();
	const auto& renderer = app->GetRenderer();

	m_pVertexShader = std::make_unique<SD::RENDER::VertexShader>(renderer, L"blinn_phong.vs.cso");
	m_pPixelShader = std::make_unique<SD::RENDER::PixelShader>(renderer, L"blinn_phong.ps.cso");

	m_pRasterizer = std::make_unique<RENDER::Rasterizer>(renderer, !material.doubleSided);

	// MASK blending is not supported yet
	const bool blendEnabled = material.alphaMode != "OPAQUE";
	m_pBlender = std::make_unique<SD::RENDER::Blender>(renderer, blendEnabled);

	// create textures
	{
		const auto textureIndex = material.pbrMetallicRoughness.baseColorTexture.index;

		if (textureIndex >= 0)
		{
			const auto& texture = model.textures[textureIndex];
			m_pDiffuseTexture = world->m_textures[texture.source];
			m_pSampler = world->m_samplers[texture.sampler];
		}
		else
		{
			m_pDiffuseTexture = std::make_shared<SD::RENDER::Texture>(renderer, L"..\\res\\textures\\base_color.dds");
			m_pSampler = std::make_shared<SD::RENDER::Sampler>(renderer);
		}

		m_pSpecularTexture = std::make_unique<SD::RENDER::Texture>(renderer, L"..\\res\\textures\\specular.dds");
	}

	CB_material materialCB;
	materialCB.shiness = 32.0f;
	m_pMaterialCB = std::make_unique<SD::RENDER::ConstantBuffer<CB_material>>(renderer, materialCB);
}

void World::Material::Bind()
{
	const auto& app = Application::GetApplication();
	const auto& renderer = app->GetRenderer();
	const auto& context = renderer->GetContext();

	D3D_DEBUG_LAYER(renderer);

	// bind shaders
	m_pVertexShader->Bind(renderer);
	m_pPixelShader->Bind(renderer);

	// bind constant buffer
	m_pMaterialCB->PSBind(renderer, 0u);

	// bind textures
	m_pDiffuseTexture->Bind(renderer, 0u);
	m_pSpecularTexture->Bind(renderer, 1u);

	// bind texture sampler
	m_pSampler->Bind(renderer);

	// bind rasterizer state
	m_pRasterizer->Bind(renderer);

	// bind Blend State
	m_pBlender->Bind(renderer);
}

World::Mesh::Mesh(const std::string& name, const uint32_t id)
	: m_name(name)
	, m_id(id)
{
}

void World::Mesh::Setup(const World* world, const tinygltf::Model& model, const tinygltf::Mesh& mesh)
{
	m_primitives.reserve(mesh.primitives.size());

	for (const auto& primitive : mesh.primitives)
	{
		m_primitives.emplace_back(std::make_unique<Primitive>(world, model, primitive));
	}
}

void World::Mesh::Draw()
{
	for (auto& primitive : m_primitives)
	{
		primitive->Draw();
	}
}

World::Primitive::Attribute::Attribute(const std::string& name)
	: m_name(name)
	, m_semanticIdx(0)
{
	if (const auto pos = m_name.find_last_of('_'); pos != m_name.npos )
	{
		auto view = std::string_view(m_name);
		view.remove_prefix(pos + 1);
		m_semanticIdx = std::stoi(view.data());

		view = std::string_view(m_name);
		view.remove_suffix(name.size() - pos);
		m_name = view;
	}
}

World::Primitive::Primitive(const World* world, const tinygltf::Model& model, const tinygltf::Primitive& primitive)
{
	const auto& app = Application::GetApplication();
	const auto& renderer = app->GetRenderer();

	m_material = world->m_materials[primitive.material];

	// setup indices
	{
		const auto accessor = model.accessors[primitive.indices];

		m_pIndexBuffer = std::static_pointer_cast<const RENDER::IndexBuffer>(world->m_buffers[accessor.bufferView]);

		m_indicesOffset = accessor.byteOffset;
		m_indicesCount = accessor.count;
	}

	// setup input layout and vertex buffers
	{
		D3D_DEBUG_LAYER(renderer);

		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;

		m_attributes.reserve(primitive.attributes.size());

		m_vertexBuffers.reserve(primitive.attributes.size());
		m_vertexStrides.reserve(primitive.attributes.size());
		m_vertexOffsets.reserve(primitive.attributes.size());

		UINT inputSlot = 0;

		// create vertex buffers
		for (const auto& [name, idx] : primitive.attributes)
		{
			if (name._Starts_with("_"))
			{
				continue;
			}

			if (name == "TANGENT")
			{
				continue;
			}

			const auto& accessor = model.accessors[idx];
			const auto& bufferView = model.bufferViews[accessor.bufferView];

			const auto& attribute = m_attributes.emplace_back(name);

			inputLayoutDesc.push_back(
				{ attribute.m_name.c_str(), static_cast<UINT>(attribute.m_semanticIdx), BUFFER_FORMATS.at({accessor.componentType, accessor.type}), inputSlot++, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			);

			m_vertexBuffers.push_back(std::static_pointer_cast<const RENDER::VertexBuffer>(world->m_buffers[accessor.bufferView]));
			m_vertexStrides.push_back(accessor.ByteStride(bufferView));
			m_vertexOffsets.push_back(accessor.byteOffset);
		}

		// create input (vertex) layout
		m_pInputLayout = std::make_unique<SD::RENDER::InputLayout>(renderer, inputLayoutDesc, m_material->m_pVertexShader->GetBytecode());
	}
}

void World::Primitive::Draw()
{
	const auto& app = Application::GetApplication();
	const auto& renderer = app->GetRenderer();
	const auto& context = renderer->GetContext();

	D3D_DEBUG_LAYER(renderer);

	m_material->Bind();

	// Bind vertex buffer
	UINT slot = 0;
	for (const auto& vertexBuffer : m_vertexBuffers)
	{
		vertexBuffer->Bind(renderer, slot, m_vertexStrides[slot], m_vertexOffsets[slot]);
		slot++;
	}

	// Bind index buffer
	m_pIndexBuffer->Bind(renderer, 0u, 0u, m_indicesOffset);

	// bind vertex layout
	m_pInputLayout->Bind(renderer);

	// Draw
	D3D_THROW_IF_INFO(context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	D3D_THROW_IF_INFO(context->DrawIndexed(m_indicesCount, 0u, 0u));

	// Unbind SRV
	ID3D11ShaderResourceView* nullSRV = nullptr;
	D3D_THROW_IF_INFO(context->PSSetShaderResources(2u, 1u, &nullSRV));
}
}  // end namespace SD::ENGINE
