#pragma once

#include <DirectXMath.h>

#include <memory>
#include <filesystem>
#include <string>

#include "space.hpp"

#include "blender.hpp"
#include "buffer.hpp"
#include "constant_buffer.hpp"
#include "frame_buffer.hpp"
#include "structured_buffer.hpp"
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
struct Mesh;
struct Primitive;
struct Scene;
struct Material;
struct Light;
}

namespace SD::ENGINE {

enum class LightType : uint8_t
{
    POINT,
    SPOT,
    DIRECTIONAL
};

class World
{
private:
    friend class SceneBrowserPanel;
    friend class NodePropertiesPanel;

    class Environment;
    class Node;
    class Mesh;
    class Primitive;
    class Material;
    class Scene;
    class Light;

#pragma warning( push )
#pragma warning( disable : 4324 )  // structure was padded due to alignment specifier
    struct PointLight
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 color;
        float intencity;
    };

    struct PointLights
    {
        alignas(16) int lightsCount;
    };
#pragma warning( pop )

    static constexpr size_t MAX_LIGHTS = 512;

public:
    World(const Space* space);
    ~World();

    void Create(const std::string& path, const std::string& environment, const DirectX::XMMATRIX& transform = DirectX::XMMatrixIdentity());

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
    void createLights(const tinygltf::Model& model);
    void createNodes(const tinygltf::Model& model);
    void createScenes(const tinygltf::Model& model);

private:
    std::unique_ptr<Timer> m_pTimer;

    const Space* m_space;

    std::vector<std::shared_ptr<RENDER::Texture>> m_textures = {};
    std::vector<std::shared_ptr<RENDER::Sampler>> m_samplers = {};
    std::vector<std::shared_ptr<RENDER::Buffer>> m_buffers = {};
    std::vector<std::shared_ptr<Material>> m_materials = {};
    std::vector<std::shared_ptr<Mesh>> m_meshes = {};
    std::vector<std::shared_ptr<Node>> m_nodes = {};
    std::vector<std::shared_ptr<Light>> m_lights = {};
    std::vector<std::shared_ptr<Scene>> m_scenes = {};

    DirectX::XMMATRIX m_transform = DirectX::XMMatrixIdentity();

    uint32_t m_selectedScene = 0;

    std::unique_ptr<SceneBrowserPanel> m_sceneBrowserPanel = nullptr;
    std::unique_ptr<NodePropertiesPanel> m_nodePropertiesPanel = nullptr;

    std::unique_ptr<World::Environment> m_environment = nullptr;
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

    void updateLights();

private:
    const std::string m_name;
    const std::uint32_t m_id;

    std::shared_ptr<Node> m_root = nullptr;

    std::unique_ptr<RENDER::StructuredBuffer<PointLight>> m_pPointLightsBuffer;
    std::unique_ptr<RENDER::ConstantBuffer<PointLights>> m_pPointLightsConstants;
};

class World::Environment
{
    struct CB_transform1
    {
        DirectX::XMMATRIX view;
    };

    struct CB_transform2
    {
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
    };

    struct CB_constants
    {
        float roughness;
        float pad[3];
    };

public:
    Environment(const std::string& name);
    ~Environment() = default;

    void Draw();

    void BindRadianceMap() const;
    void BindIrradianceMap() const;
    void BindPrefilterMap() const;
    void BindBRDFLUT() const;

    void CreateRadianceMap();
    void ConvolveIrradianceMap();
    void CreatePrefilterMap();
    void ConvolveBRDF();

private:
    std::unique_ptr<RENDER::Texture> m_environment;

    std::unique_ptr<RENDER::CubeFrameBuffer> m_radianceMap = nullptr;
    std::unique_ptr<RENDER::CubeFrameBuffer> m_irradianceMap = nullptr;
    std::unique_ptr<RENDER::CubeFrameBuffer> m_prefilterMap = nullptr;
    std::unique_ptr<RENDER::FrameBuffer> m_brdfLUT = nullptr;

    std::shared_ptr<RENDER::Sampler> m_environmentSampler = nullptr;
    std::shared_ptr<RENDER::Sampler> m_brdfSampler = nullptr;

    std::shared_ptr<RENDER::Rasterizer> m_pRasterizer = nullptr;
    std::shared_ptr<RENDER::Blender> m_pBlender = nullptr;

    std::unique_ptr<RENDER::VertexShader> m_pCubemapVertexShader = nullptr;
    std::unique_ptr<RENDER::PixelShader> m_pEquirectangularToCubemapPixelShader = nullptr;
    std::unique_ptr<RENDER::PixelShader> m_pIrradianceConvolutionPixelShader = nullptr;

    std::unique_ptr<RENDER::VertexShader> m_pPrefilterVertexShader = nullptr;
    std::unique_ptr<RENDER::PixelShader> m_pPrefilterPixelShader = nullptr;

    std::unique_ptr<RENDER::VertexShader> m_pBackgroundVertexShader = nullptr;
    std::unique_ptr<RENDER::PixelShader> m_pBackgroundPixelShader = nullptr;

    std::unique_ptr<RENDER::VertexShader> m_pConvolveBRDFVertexShader = nullptr;
    std::unique_ptr<RENDER::PixelShader> m_pConvolveBRDFPixelShader = nullptr;

    std::unique_ptr<RENDER::ConstantBuffer<CB_transform1>> m_transformCB1 = nullptr;
    std::unique_ptr<RENDER::ConstantBuffer<CB_transform2>> m_transformCB2 = nullptr;
    std::unique_ptr<RENDER::ConstantBuffer<CB_constants>> m_constantsCB = nullptr;

    std::unique_ptr<RENDER::VertexBuffer> m_pVertexBufferCube = nullptr;
    std::unique_ptr<RENDER::IndexBuffer> m_pIndexBufferCube = nullptr;
    std::unique_ptr<RENDER::InputLayout> m_pInputLayoutCube = nullptr;

    std::unique_ptr<RENDER::VertexBuffer> m_pVertexBufferQuad = nullptr;
    std::unique_ptr<RENDER::IndexBuffer> m_pIndexBufferQuad = nullptr;
    std::unique_ptr<RENDER::InputLayout> m_pInputLayoutQuad = nullptr;
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

    void CollectLights(std::vector<PointLight>& lights);

private:
    const std::string m_name;
    const std::uint32_t m_id;

    DirectX::XMMATRIX m_localTransform = DirectX::XMMatrixIdentity();
    DirectX::XMMATRIX m_worldTransform = DirectX::XMMatrixIdentity();

    DirectX::XMVECTOR m_originalScale = {};
    DirectX::XMVECTOR m_originalRotation = {};
    DirectX::XMVECTOR m_originalTranslation = {};

    std::shared_ptr<Mesh> m_mesh = nullptr;
    std::shared_ptr<Light> m_light = nullptr;

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
        // TODO: alignment
        DirectX::XMFLOAT4 baseColorFactor;
        float normalMapScale;
        float metallicFactor;
        alignas(8) float roughnessFactor;
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

    std::shared_ptr<const RENDER::Texture> m_pAlbedoTexture = nullptr;
    std::shared_ptr<const RENDER::Texture> m_pNormalTexture = nullptr;
    std::shared_ptr<const RENDER::Texture> m_pMetallicRoughnessTexture = nullptr;

    std::shared_ptr<RENDER::Sampler> m_pAlbedoSampler = nullptr;
    std::shared_ptr<RENDER::Sampler> m_pNormalSampler = nullptr;
    std::shared_ptr<RENDER::Sampler> m_pMetallicRoughnessSampler = nullptr;

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

class World::Light
{
private:
    friend class LightPropertiesPanel;
    friend class Node; // TODO

public:
    Light(const std::string& name);
    ~Light() = default;

    void Setup(const tinygltf::Light& light);

private:
    const std::string m_name;
    
    DirectX::XMFLOAT3 m_color;
    float m_intencity;
    LightType m_type;
};
}  // end namespace SD::ENGINE
