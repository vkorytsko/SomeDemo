#pragma once

#include <DirectXMath.h>

#include <memory>
#include <filesystem>
#include <string>

#include "buffer.hpp"
#include "constant_buffer.hpp"
#include "index_buffer.hpp"
#include "input_layout.hpp"
#include "pixel_shader.hpp"
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
    class Node;
    class Mesh;
    class Primitive;
    class Material;
    class Scene;

public:
    World(const std::string& path, const DirectX::XMMATRIX& transform = DirectX::XMMatrixIdentity());
    ~World() = default;

    void Simulate(float dt);
    void Update(float dt);
    void Draw();

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
    std::vector<std::shared_ptr<RENDER::Texture>> m_textures = {};
    std::vector<std::shared_ptr<RENDER::Sampler>> m_samplers = {};
    std::vector<std::shared_ptr<RENDER::Buffer>> m_buffers = {};
    std::vector<std::shared_ptr<Material>> m_materials = {};
    std::vector<std::shared_ptr<Mesh>> m_meshes = {};
    std::vector<std::shared_ptr<Node>> m_nodes = {};
    std::vector<std::shared_ptr<Scene>> m_scenes = {};

    DirectX::XMMATRIX m_transform = DirectX::XMMatrixIdentity();
};

class World::Scene
{
public:
    Scene(const World* world, const tinygltf::Model& model, const tinygltf::Scene& scene);
    ~Scene() = default;

    void Simulate(float dt);
    void Update(float dt);
    void Draw();

    std::shared_ptr<Node> m_root = nullptr;

private:
    void buildHierarchy(
        const World* world,
        const tinygltf::Model& model,
        const std::shared_ptr<Node>& parent,
        const int nodeIdx) const;
    const std::vector<std::shared_ptr<Node>> getChildren(
        const World* world,
        const std::vector<int>& children) const;
};

class World::Node
{
private:
    friend class Scene;

    struct CB_transform
    {
        DirectX::XMMATRIX model;
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
        alignas(16) DirectX::XMFLOAT3 viewPosition;
    };

public:
    Node(const DirectX::XMMATRIX& transform);
    Node(const World* world, const tinygltf::Node& node);
    ~Node() = default;

    void Simulate(float dt);
    void Update(float dt);
    void Draw();

private:
    DirectX::XMMATRIX m_localTransform = DirectX::XMMatrixIdentity();
    DirectX::XMMATRIX m_worldTransform = DirectX::XMMatrixIdentity();

    std::shared_ptr<Mesh> m_mesh = nullptr;

    std::weak_ptr<Node> m_parent;
    std::vector<std::shared_ptr<Node>> m_children = {};

    std::unique_ptr<RENDER::ConstantBuffer<CB_transform>> m_pTransformCB = nullptr;
};

class World::Material
{
private:
    struct CB_material
    {
        alignas(16) float shiness;
    };

public:
    friend class Primitive;

    Material(const World* world, const tinygltf::Model& model, const tinygltf::Material& material);
    ~Material() = default;

    void Bind();

private:
    std::unique_ptr<RENDER::PixelShader> m_pPixelShader = nullptr;
    std::unique_ptr<RENDER::VertexShader> m_pVertexShader = nullptr;

    std::shared_ptr<const RENDER::Texture> m_pDiffuseTexture = nullptr;
    std::unique_ptr<RENDER::Texture> m_pSpecularTexture = nullptr;

    std::shared_ptr<RENDER::Sampler> m_pSampler = nullptr;

    std::unique_ptr<RENDER::ConstantBuffer<CB_material>> m_pMaterialCB = nullptr;
};

class World::Mesh
{
public:
    Mesh(const World* world, const tinygltf::Model& model, const tinygltf::Mesh& mesh);
    ~Mesh() = default;

    void Draw();

private:
    std::vector<std::unique_ptr<Primitive>> m_primitives = {};
};

class World::Primitive
{
private:
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
