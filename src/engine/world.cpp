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
#pragma warning( push, 0 )
#include "tiny_gltf.h"
#include <DirectXMath.h>
#pragma warning( pop )


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

const std::unordered_map<std::string, SD::ENGINE::LightType> LIGHT_TYPES_MAP = {
	{"point", SD::ENGINE::LightType::POINT},
	{"spot", SD::ENGINE::LightType::SPOT},
	{"directional", SD::ENGINE::LightType::DIRECTIONAL},
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

void World::Create(const std::string& path, const std::string& environment, const DirectX::XMMATRIX& transform)
{
	m_transform = transform;

	const auto& model = load(path);

	createBuffers(model);
	createTextures(model, std::filesystem::path(path).remove_filename());
	createSamplers(model);
	createMaterials(model);
	createMeshes(model);
	createLights(model);
	createNodes(model);
	createScenes(model);

	m_selectedScene = model.defaultScene;

	m_sceneBrowserPanel = std::make_unique<SceneBrowserPanel>();
	m_nodePropertiesPanel = std::make_unique<NodePropertiesPanel>();

	m_environment = std::make_unique<Environment>(environment);
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

	m_environment->CreateRadianceMap();
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
	const auto& renderSystem = app->GetRenderSystem();

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
		m_buffers.back()->create(renderSystem->GetRenderer(), buffer.data.data() + bufferView.byteOffset, bufferView.byteLength);
	}

	std::clog << "Buffers created: " << m_pTimer->GetDelta() << " s." << std::endl;
}

void World::createTextures(const tinygltf::Model& model, const std::filesystem::path& dir)
{
	std::clog << "Create textures!" << std::endl;

	const auto& app = Application::GetApplication();
	const auto& renderSystem = app->GetRenderSystem();

	for (const auto& image : model.images)
	{
		const auto path = dir.string() + image.uri;
		m_textures.emplace_back(std::make_shared<RENDER::Texture>(renderSystem->GetRenderer(), AToWstring(path)));
	}

	std::clog << "Textures created: " << m_pTimer->GetDelta() << " s." << std::endl;
}

void World::createSamplers(const tinygltf::Model& model)
{
	std::clog << "Create samplers!" << std::endl;

	const auto& app = Application::GetApplication();
	const auto& renderSystem = app->GetRenderSystem();

#pragma warning(disable:4189)  // local variable is initialized but not referenced
	for (const auto& sampler : model.samplers)
	{
		m_samplers.emplace_back(std::make_shared<RENDER::Sampler>(renderSystem->GetRenderer()));
	}
#pragma warning(default:4189)

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

void World::createLights(const tinygltf::Model& model)
{
	std::clog << "Create lights!" << std::endl;

	for (const auto& light : model.lights)
	{
		const auto id = static_cast<uint32_t>(m_nodes.size());
		const std::string name = light.name.empty() ? "Light " + std::to_string(id) : light.name;
		m_lights.emplace_back(std::make_shared<Light>(name))->Setup(light);
	}

	std::clog << "Lights created: " << m_pTimer->GetDelta() << " s." << std::endl;
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

	const auto& app = Application::GetApplication();
	const auto& renderSystem = app->GetRenderSystem();

	// create structured and constant buffers
	std::vector<PointLight> lights;
	lights.reserve(MAX_LIGHTS);
	m_pPointLightsBuffer = std::make_unique<RENDER::StructuredBuffer<PointLight>>(renderSystem->GetRenderer(), lights);

	PointLights lightsConstants;
	lightsConstants.lightsCount = 0;
	m_pPointLightsConstants = std::make_unique<RENDER::ConstantBuffer<PointLights>>(renderSystem->GetRenderer(), lightsConstants);
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

	updateLights();
}

void World::Scene::Draw()
{
	const auto& app = Application::GetApplication();
	const auto& renderSystem = app->GetRenderSystem();

	m_pPointLightsBuffer->PSBind(renderSystem->GetRenderer(), 3);
	m_pPointLightsConstants->PSBind(renderSystem->GetRenderer(), 2);

	m_root->Draw();
}

void World::Scene::updateLights()
{
	auto& lights = m_pPointLightsBuffer->GetData();
	lights.clear();
	lights.reserve(MAX_LIGHTS);
	m_root->CollectLights(lights);

	auto* lightsConstants = m_pPointLightsConstants->GetData();
	lightsConstants->lightsCount = static_cast<int>(lights.size());

	const auto& app = Application::GetApplication();
	const auto& renderSystem = app->GetRenderSystem();

	m_pPointLightsBuffer->Update(renderSystem->GetRenderer());
	m_pPointLightsConstants->Update(renderSystem->GetRenderer());
}

World::Environment::Environment(const std::string& name)
{
	const auto& app = Application::GetApplication();
	const auto& renderSystem = app->GetRenderSystem();

	m_environment = std::make_unique<RENDER::Texture>(renderSystem->GetRenderer(), AToWstring(name));

	static constexpr float size = 1024.0f;

	m_radianceMap = std::make_unique<RENDER::CubeFrameBuffer>(renderSystem->GetRenderer(), size);
	m_irradianceMap = std::make_unique<RENDER::CubeFrameBuffer>(renderSystem->GetRenderer(), size);

	m_pRadianceVertexShader = std::make_unique<SD::RENDER::VertexShader>(renderSystem->GetRenderer(), L"environment.vs.cso");
	m_pRadiancePixelShader = std::make_unique<SD::RENDER::PixelShader>(renderSystem->GetRenderer(), L"environment.ps.cso");

	m_cubemapSampler = std::make_shared<RENDER::Sampler>(renderSystem->GetRenderer());

	m_pRasterizer = std::make_unique<RENDER::Rasterizer>(renderSystem->GetRenderer(), false);

	m_pBlender = std::make_unique<SD::RENDER::Blender>(renderSystem->GetRenderer(), false);

	const std::vector<float> vertices =
	{
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
	};
	m_pVertexBuffer = std::make_unique<RENDER::VertexBuffer>();
	m_pVertexBuffer->create(
		renderSystem->GetRenderer(), vertices.data(), 
		sizeof(decltype(vertices)::value_type) * vertices.size()
	);

	const std::vector<unsigned short> indices =
	{
		0, 2, 1,  2, 3, 1,
		1, 3, 5,  3, 7, 5,
		2, 6, 3,  3, 6, 7,
		4, 5, 7,  4, 7, 6,
		0, 4, 2,  2, 4, 6,
		0, 1, 4,  1, 5, 4,
	};
	m_pIndexBuffer = std::make_unique<RENDER::IndexBuffer>();
	m_pIndexBuffer->create(
		renderSystem->GetRenderer(), indices.data(),
		sizeof(decltype(indices)::value_type) * indices.size()
	);

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	inputLayoutDesc.push_back(
		{ "position", 0u,
		DXGI_FORMAT_R32G32B32_FLOAT, 0u,D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA, 0 }
	);

	// create input (vertex) layout
	m_pInputLayout = std::make_unique<SD::RENDER::InputLayout>(
		renderSystem->GetRenderer(), inputLayoutDesc, m_pRadianceVertexShader->GetBytecode()
	);

	CB_transform transformCB;
	m_transformCB = std::make_unique<SD::RENDER::ConstantBuffer<CB_transform>>(renderSystem->GetRenderer(), transformCB);
}

void World::Environment::BindRadianceMap() const
{
	
}

void World::Environment::BindIrradianceMap() const
{
}

void World::Environment::CreateRadianceMap()
{
	const auto& app = Application::GetApplication();
	const auto& renderSystem = app->GetRenderSystem();
	const auto& renderer = renderSystem->GetRenderer();

	D3D_DEBUG_LAYER(renderer);

	// Begin
	{
		m_radianceMap->bind(renderer);  // ???

		const float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		for (uint8_t face = 0; face < 6; ++face)
		{
			D3D_THROW_IF_INFO(renderer->GetContext()->ClearRenderTargetView(m_radianceMap->getRTV(face).Get(), color));
			D3D_THROW_IF_INFO(renderer->GetContext()->ClearDepthStencilView(m_radianceMap->getDSV(face).Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u));
		}

		D3D_THROW_IF_INFO(renderer->GetContext()->ClearRenderTargetView(renderer->GetRenderTargetView().Get(), color));
	}

	// Draw
	{
		// configure viewport
		D3D11_VIEWPORT vp;
		vp.Width = static_cast<float>(m_radianceMap->size());
		vp.Height = static_cast<float>(m_radianceMap->size());
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		D3D_THROW_IF_INFO(renderer->GetContext()->RSSetViewports(1u, &vp));

		// bind shaders
		m_pRadianceVertexShader->Bind(renderer);
		m_pRadiancePixelShader->Bind(renderer);

		// bind constant buffer
		m_transformCB->VSBind(renderer, 0u);

		// bind textures
		m_environment->Bind(renderer, 0u);

		// bind texture
		m_environment->Bind(renderer, 0u);

		// bind texture samplers
		m_cubemapSampler->Bind(renderer, 0u);

		// bind rasterizer state
		m_pRasterizer->Bind(renderer);

		// bind Blend State
		m_pBlender->Bind(renderer);

		// Bind vertex buffer
		m_pVertexBuffer->Bind(renderer, 0u, 12u, 0u);

		// Bind index buffer
		m_pIndexBuffer->Bind(renderer, 0u, 0u, 0u);

		// bind vertex layout
		m_pInputLayout->Bind(renderer);

		const auto& context = renderer->GetContext();

		D3D_THROW_IF_INFO(context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));


		const DirectX::XMMATRIX views[] = {
			DirectX::XMMatrixLookAtLH(
				{0.0f, 0.0f, 0.0f},
				{1.0f, 0.0f, 0.0f},
				{ 0.0f, 1.0f, 0.0f }),
			DirectX::XMMatrixLookAtLH(
				{0.0f, 0.0f, 0.0f},
				{-1.0f, 0.0f, 0.0f},
				{ 0.0f, 1.0f, 0.0f }),
			DirectX::XMMatrixLookAtLH(
				{0.0f, 0.0f, 0.0f},
				{0.0f, 1.0f, 0.0f},
				{ 0.0f, 0.0f, 1.0f }),
			DirectX::XMMatrixLookAtLH(
				{0.0f, 0.0f, 0.0f},
				{0.0f, -1.0f, 0.0f},
				{ 0.0f, 0.0f, -1.0f }),
			DirectX::XMMatrixLookAtLH(
				{0.0f, 0.0f, 0.0f},
				{0.0f, 0.0f, 1.0f},
				{ 0.0f, 1.0f, 0.0f }),
			DirectX::XMMatrixLookAtLH(
				{0.0f, 0.0f, 0.0f},
				{0.0f, 0.0f, -1.0f},
				{ 0.0f, 1.0f, 0.0f }),
		};

		for (uint8_t face = 0; face < 6; ++face)
		{
			D3D_THROW_IF_INFO(renderer->GetContext()->OMSetRenderTargets(1u, m_radianceMap->getRTV(face).GetAddressOf(), nullptr));

			const auto& transformCB = m_transformCB->GetData();
			transformCB->view = views[face];
			m_transformCB->Update(renderSystem->GetRenderer());

			D3D_THROW_IF_INFO(context->DrawIndexed(36u, 0u, 0u));

			// Unbind SRV
			ID3D11ShaderResourceView* nullSRV = nullptr;
			D3D_THROW_IF_INFO(context->PSSetShaderResources(2u, 1u, &nullSRV));
		}
	}
}

void World::Environment::ConvolveIrradianceMap()
{
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

	if (node.light >= 0)
	{
		m_light = world->m_lights[node.light];
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
	const auto& renderSystem = app->GetRenderSystem();

	// create constant buffers
	CB_transform transformCB;
	m_pTransformCB = std::make_unique<SD::RENDER::ConstantBuffer<CB_transform>>(renderSystem->GetRenderer(), transformCB);
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
	const auto& renderSystem = app->GetRenderSystem();

	if (m_mesh)
	{
		// update transform constant buffer
		{
			const auto& transformCB = m_pTransformCB->GetData();
			transformCB->model = m_worldTransform;
			transformCB->view = camera->getView();
			transformCB->projection = camera->getProjection();
			transformCB->viewPosition = camera->getPosition();
			m_pTransformCB->Update(renderSystem->GetRenderer());
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
	const auto& renderSystem = app->GetRenderSystem();

	if (m_mesh)
	{
		m_pTransformCB->VSBind(renderSystem->GetRenderer(), 0u);

		m_mesh->Draw();
	}

	for (const auto& child : m_children)
	{
		child->Draw();
	}
}

void World::Node::CollectLights(std::vector<PointLight>& lights)
{
	if (m_light)
	{
		if (m_light->m_type == LightType::POINT)
		{
			PointLight& light = lights.emplace_back();

			DirectX::XMVECTOR translation, rotation, scale;
			DirectX::XMMatrixDecompose(&scale, &rotation, &translation, m_worldTransform);
			DirectX::XMStoreFloat3(&light.position, translation);

			light.color = m_light->m_color;
			light.intencity = m_light->m_intencity;
		}
	}

	for (const auto& child : m_children)
	{
		child->CollectLights(lights);
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
	const auto& renderSystem = app->GetRenderSystem();

	m_pVertexShader = std::make_unique<SD::RENDER::VertexShader>(renderSystem->GetRenderer(), L"pbr.vs.cso");
	m_pPixelShader = std::make_unique<SD::RENDER::PixelShader>(renderSystem->GetRenderer(), L"pbr.ps.cso");

	m_pRasterizer = std::make_unique<RENDER::Rasterizer>(renderSystem->GetRenderer(), !material.doubleSided);

	// MASK blending is not supported yet
	const bool blendEnabled = material.alphaMode != "OPAQUE";
	m_pBlender = std::make_unique<SD::RENDER::Blender>(renderSystem->GetRenderer(), blendEnabled);

	// create textures
	{
		// albedo
		{
			const auto textureIndex = material.pbrMetallicRoughness.baseColorTexture.index;
			if (textureIndex >= 0)
			{
				const auto& texture = model.textures[textureIndex];
				m_pAlbedoTexture = world->m_textures[texture.source];
				m_pAlbedoSampler = world->m_samplers[texture.sampler];
			}
			else
			{
				static const std::string albedo = SD_RES_DIR + std::string("textures\\albedo.dds");
				m_pAlbedoTexture = std::make_shared<SD::RENDER::Texture>(renderSystem->GetRenderer(), AToWstring(albedo));
				m_pAlbedoSampler = std::make_shared<SD::RENDER::Sampler>(renderSystem->GetRenderer());
			}
		}
		// normal
		{
			const auto textureIndex = material.normalTexture.index;
			if (textureIndex >= 0)
			{
				const auto& texture = model.textures[textureIndex];
				m_pNormalTexture = world->m_textures[texture.source];
				m_pNormalSampler = world->m_samplers[texture.sampler];
			}
			else
			{
				static const std::string normal = SD_RES_DIR + std::string("textures\\normal.dds");
				m_pNormalTexture = std::make_shared<SD::RENDER::Texture>(renderSystem->GetRenderer(), AToWstring(normal));
				m_pNormalSampler = std::make_shared<SD::RENDER::Sampler>(renderSystem->GetRenderer());
			}
		}
		// metallicRoughness
		{
			const auto textureIndex = material.pbrMetallicRoughness.metallicRoughnessTexture.index;
			if (textureIndex >= 0)
			{
				const auto& texture = model.textures[textureIndex];
				m_pMetallicRoughnessTexture = world->m_textures[texture.source];
				m_pMetallicRoughnessSampler = world->m_samplers[texture.sampler];
			}
			else
			{
				static const std::string metallicRoughness = SD_RES_DIR + std::string("textures\\metallicRoughness.dds");
				m_pMetallicRoughnessTexture = std::make_shared<SD::RENDER::Texture>(renderSystem->GetRenderer(), AToWstring(metallicRoughness));
				m_pMetallicRoughnessSampler = std::make_shared<SD::RENDER::Sampler>(renderSystem->GetRenderer());
			}
		}
	}

	CB_material materialCB;
	materialCB.normalMapScale = static_cast<float>(material.normalTexture.scale);
	materialCB.metallicFactor = static_cast<float>(material.pbrMetallicRoughness.metallicFactor);
	materialCB.roughnessFactor = static_cast<float>(material.pbrMetallicRoughness.roughnessFactor);
	materialCB.baseColorFactor = DirectX::XMFLOAT4(
		static_cast<float>(material.pbrMetallicRoughness.baseColorFactor[0]),
		static_cast<float>(material.pbrMetallicRoughness.baseColorFactor[1]),
		static_cast<float>(material.pbrMetallicRoughness.baseColorFactor[2]),
		static_cast<float>(material.pbrMetallicRoughness.baseColorFactor[3])
	);
	m_pMaterialCB = std::make_unique<SD::RENDER::ConstantBuffer<CB_material>>(renderSystem->GetRenderer(), materialCB);
}

void World::Material::Bind()
{
	const auto& app = Application::GetApplication();
	const auto& renderSystem = app->GetRenderSystem();

	// bind shaders
	m_pVertexShader->Bind(renderSystem->GetRenderer());
	m_pPixelShader->Bind(renderSystem->GetRenderer());

	// bind constant buffer
	m_pMaterialCB->PSBind(renderSystem->GetRenderer(), 0u);

	// bind textures
	m_pAlbedoTexture->Bind(renderSystem->GetRenderer(), 0u);
	m_pNormalTexture->Bind(renderSystem->GetRenderer(), 1u);
	m_pMetallicRoughnessTexture->Bind(renderSystem->GetRenderer(), 2u);

	// bind texture samplers
	m_pAlbedoSampler->Bind(renderSystem->GetRenderer(), 0u);
	m_pNormalSampler->Bind(renderSystem->GetRenderer(), 1u);
	m_pMetallicRoughnessSampler->Bind(renderSystem->GetRenderer(), 2u);

	// bind rasterizer state
	m_pRasterizer->Bind(renderSystem->GetRenderer());

	// bind Blend State
	m_pBlender->Bind(renderSystem->GetRenderer());
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
	const auto& renderSystem = app->GetRenderSystem();

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
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;

		m_attributes.reserve(primitive.attributes.size());

		m_vertexBuffers.reserve(primitive.attributes.size());
		m_vertexStrides.reserve(primitive.attributes.size());
		m_vertexOffsets.reserve(primitive.attributes.size());

		UINT inputSlot = 0;

		// create vertex buffers
		for (const auto& [name, idx] : primitive.attributes)
		{
			const auto& accessor = model.accessors[idx];
			const auto& bufferView = model.bufferViews[accessor.bufferView];

			const auto& attribute = m_attributes.emplace_back(name);
			const DXGI_FORMAT format = BUFFER_FORMATS.at({ accessor.componentType, accessor.type });

			inputLayoutDesc.push_back(
				{ attribute.m_name.c_str(), static_cast<UINT>(attribute.m_semanticIdx),
				format, inputSlot++,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			);

			m_vertexBuffers.push_back(std::static_pointer_cast<const RENDER::VertexBuffer>(world->m_buffers[accessor.bufferView]));
			m_vertexStrides.push_back(accessor.ByteStride(bufferView));
			m_vertexOffsets.push_back(accessor.byteOffset);
		}

		// create input (vertex) layout
		m_pInputLayout = std::make_unique<SD::RENDER::InputLayout>(renderSystem->GetRenderer(), inputLayoutDesc, m_material->m_pVertexShader->GetBytecode());
	}
}

void World::Primitive::Draw()
{
	const auto& app = Application::GetApplication();
	const auto& renderSystem = app->GetRenderSystem();
	const auto& context = renderSystem->GetRenderer()->GetContext();

	D3D_DEBUG_LAYER(renderSystem->GetRenderer());

	m_material->Bind();

	// Bind vertex buffer
	UINT slot = 0;
	for (const auto& vertexBuffer : m_vertexBuffers)
	{
		vertexBuffer->Bind(renderSystem->GetRenderer(), slot, static_cast<UINT>(m_vertexStrides[slot]), static_cast<UINT>(m_vertexOffsets[slot]));
		slot++;
	}

	// Bind index buffer
	m_pIndexBuffer->Bind(renderSystem->GetRenderer(), 0u, 0u, static_cast<UINT>(m_indicesOffset));

	// bind vertex layout
	m_pInputLayout->Bind(renderSystem->GetRenderer());

	// Draw
	D3D_THROW_IF_INFO(context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	D3D_THROW_IF_INFO(context->DrawIndexed(static_cast<UINT>(m_indicesCount), 0u, 0u));

	// Unbind SRV
	ID3D11ShaderResourceView* nullSRV = nullptr;
	D3D_THROW_IF_INFO(context->PSSetShaderResources(2u, 1u, &nullSRV));
}

World::Light::Light(const std::string& name)
	: m_name(name)
{
}

void World::Light::Setup(const tinygltf::Light& light)
{
	m_color = {
		static_cast<float>(light.color[0]),
		static_cast<float>(light.color[1]),
		static_cast<float>(light.color[2])
	};

	m_type = LIGHT_TYPES_MAP.at(light.type);
	//m_intencity = static_cast<float>(light.intensity);
	m_intencity = 8;
}
}  // end namespace SD::ENGINE
