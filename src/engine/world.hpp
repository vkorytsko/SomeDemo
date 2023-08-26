#pragma once

#include <DirectXMath.h>

#include <memory>
#include <filesystem>
#include <string>

#include "space.hpp"

#include "blender.hpp"
#include "buffer.hpp"
#include "constant_buffer.hpp"
#include "index_buffer.hpp"
#include "input_layout.hpp"
#include "pixel_shader.hpp"
#include "rasterizer.hpp"
#include "sampler.hpp"
#include "texture.hpp"
#include "vertex_buffer.hpp"
#include "vertex_shader.hpp"


namespace tinygltf
{
class Model;
class Node;
class Mesh;
class Primitive;
class Scene;
class Material;
}

namespace SD::ENGINE {

class World
{
private:
    friend class SceneBrowserPanel;
    friend class NodePropertiesPanel;

    class Node;
    class Mesh;
    class Primitive;
    class Material;
    class Scene;

public:
    World(const Space* space);
    ~World();

    void Setup(const std::string& path, const DirectX::XMMATRIX& transform = DirectX::XMMatrixIdentity());

    void Simulate(float dt);
    void Update(float dt);
    void Draw();
    void DrawImGui();

private:
    tinygltf::Model load(const std::string& path) const;

    void createBuffers(const tinygltf::Model& model);
    void createTextures(const tinygltf::Model& model, const std::filesystem::path& dir);
    void createSamplers(const tinygltf::Model& model);
    void createMaterials(const tinygltf::Model& model);
    void createMeshes(const tinygltf::Model& model);
    void createNodes(const tinygltf::Model& model);
    void createScenes(const tinygltf::Model& model);

private:
    const Space* m_space;

    std::vector<std::shared_ptr<RENDER::Texture>> m_textures = {};
    std::vector<std::shared_ptr<RENDER::Sampler>> m_samplers = {};
    std::vector<std::shared_ptr<RENDER::Buffer>> m_buffers = {};
    std::vector<std::shared_ptr<Material>> m_materials = {};
    std::vector<std::shared_ptr<Mesh>> m_meshes = {};
    std::vector<std::shared_ptr<Node>> m_nodes = {};
    std::vector<std::shared_ptr<Scene>> m_scenes = {};

    DirectX::XMMATRIX m_transform = DirectX::XMMatrixIdentity();

    uint32_t m_selectedScene = 0;

    std::unique_ptr<SceneBrowserPanel> m_sceneBrowserPanel = nullptr;
    std::unique_ptr<NodePropertiesPanel> m_nodePropertiesPanel = nullptr;
};

class World::Scene
{
private:
    friend class SceneBrowserPanel;

public:
    Scene(const std::string& name, const uint32_t id);
    ~Scene() = default;

    void Setup(const World* world, const tinygltf::Model& model, const tinygltf::Scene& scene);

    void Simulate(float dt);
    void Update(float dt);
    void Draw();

private:
    void buildHierarchy(
        const World* world,
        const tinygltf::Model& model,
        const std::shared_ptr<Node>& parent,
        const int nodeIdx) const;
    const std::vector<std::shared_ptr<Node>> getChildren(
        const World* world,
        const std::vector<int>& children) const;

private:
    const std::string m_name;
    const std::uint32_t m_id;

    std::shared_ptr<Node> m_root = nullptr;
};

class World::Node
{
private:
    friend class Scene;
    friend class SceneBrowserPanel;
    friend class NodePropertiesPanel;

    struct CB_transform
    {
        DirectX::XMMATRIX model;
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
        alignas(16) DirectX::XMFLOAT3 viewPosition;
    };

public:
    Node(const std::string& name, const uint32_t id, const DirectX::XMMATRIX& transform = DirectX::XMMatrixIdentity());
    ~Node() = default;

    void Setup(const World* world, const tinygltf::Node& node);

    void Simulate(float dt);
    void Update(float dt);
    void Draw();

private:
    const std::string m_name;
    const std::uint32_t m_id;

    DirectX::XMMATRIX m_localTransform = DirectX::XMMatrixIdentity();
    DirectX::XMMATRIX m_worldTransform = DirectX::XMMatrixIdentity();

    DirectX::XMVECTOR m_originalScale = {};
    DirectX::XMVECTOR m_originalRotation = {};
    DirectX::XMVECTOR m_originalTranslation = {};

    std::shared_ptr<Mesh> m_mesh = nullptr;

    std::weak_ptr<Node> m_parent;
    std::vector<std::shared_ptr<Node>> m_children = {};

    std::unique_ptr<RENDER::ConstantBuffer<CB_transform>> m_pTransformCB = nullptr;
};

class World::Material
{
private:
    friend class Primitive;
    friend class NodePropertiesPanel;

    struct CB_material
    {
        alignas(16) float shiness;
    };

public:
    Material(const std::string& name, const uint32_t id);
    ~Material() = default;

    void Setup(const World* world, const tinygltf::Model& model, const tinygltf::Material& material);

    void Bind();

private:
    const std::string m_name;
    const std::uint32_t m_id;

    std::unique_ptr<RENDER::PixelShader> m_pPixelShader = nullptr;
    std::unique_ptr<RENDER::VertexShader> m_pVertexShader = nullptr;

    std::shared_ptr<const RENDER::Texture> m_pDiffuseTexture = nullptr;
    std::unique_ptr<RENDER::Texture> m_pSpecularTexture = nullptr;

    std::shared_ptr<RENDER::Sampler> m_pSampler = nullptr;
    std::shared_ptr<RENDER::Rasterizer> m_pRasterizer = nullptr;
    std::shared_ptr<RENDER::Blender> m_pBlender = nullptr;

    std::unique_ptr<RENDER::ConstantBuffer<CB_material>> m_pMaterialCB = nullptr;
};

class World::Mesh
{
private:
    friend class NodePropertiesPanel;

public:
    Mesh(const std::string& name, const uint32_t id);
    ~Mesh() = default;

    void Setup(const World* world, const tinygltf::Model& model, const tinygltf::Mesh& mesh);

    void Draw();

private:
    const std::string m_name;
    const std::uint32_t m_id;

    std::vector<std::unique_ptr<Primitive>> m_primitives = {};
};

class World::Primitive
{
private:
    friend class NodePropertiesPanel;

    struct Attribute
    {
        Attribute(const std::string& name);

        std::string m_name;
        std::size_t m_semanticIdx;
    };

public:
    Primitive(const World* world, const tinygltf::Model& model, const tinygltf::Primitive& primitive);
    ~Primitive() = default;

    void Draw();

private:
    std::shared_ptr<Material> m_material = nullptr;

    std::shared_ptr<const RENDER::IndexBuffer> m_pIndexBuffer = nullptr;
    size_t m_indicesCount = 0;
    size_t m_indicesOffset = 0;

    std::vector<Attribute> m_attributes = {};
    std::vector<std::shared_ptr<const RENDER::VertexBuffer>> m_vertexBuffers = {};
    std::vector<size_t> m_vertexStrides = {};
    std::vector<size_t> m_vertexOffsets = {};

    std::unique_ptr<RENDER::InputLayout> m_pInputLayout = nullptr;
};
}  // end namespace SD::ENGINE
