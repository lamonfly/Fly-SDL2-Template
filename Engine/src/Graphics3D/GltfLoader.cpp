#include "GltfLoader.h"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#pragma warning(push, 0)
#include <stb_image.h>
#include <tiny_gltf.h>
#pragma warning(pop)

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdio>
#include <cstring>
#include <functional>

namespace
{
	// Floats from a float accessor, components per element
	std::vector<float> ReadFloats(const tinygltf::Model& model, int accessorIndex, int& components)
	{
		std::vector<float> out;
		components = 0;
		if (accessorIndex < 0 || accessorIndex >= static_cast<int>(model.accessors.size())) return out;

		const tinygltf::Accessor& acc = model.accessors[accessorIndex];
		if (acc.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT || acc.bufferView < 0) return out;

		const tinygltf::BufferView& view = model.bufferViews[acc.bufferView];
		const tinygltf::Buffer& buffer = model.buffers[view.buffer];
		components = tinygltf::GetNumComponentsInType(acc.type);
		size_t elemSize = sizeof(float) * components;
		size_t stride = view.byteStride ? view.byteStride : elemSize;
		const unsigned char* base = buffer.data.data() + view.byteOffset + acc.byteOffset;

		out.resize(acc.count * components);
		for (size_t i = 0; i < acc.count; i++)
			std::memcpy(out.data() + i * components, base + i * stride, elemSize);
		return out;
	}

	std::vector<uint32_t> ReadIndices(const tinygltf::Model& model, int accessorIndex)
	{
		std::vector<uint32_t> out;
		if (accessorIndex < 0 || accessorIndex >= static_cast<int>(model.accessors.size())) return out;

		const tinygltf::Accessor& acc = model.accessors[accessorIndex];
		if (acc.bufferView < 0) return out;
		const tinygltf::BufferView& view = model.bufferViews[acc.bufferView];
		const tinygltf::Buffer& buffer = model.buffers[view.buffer];
		const unsigned char* base = buffer.data.data() + view.byteOffset + acc.byteOffset;

		size_t elemSize = tinygltf::GetComponentSizeInBytes(acc.componentType);
		size_t stride = view.byteStride ? view.byteStride : elemSize;

		out.resize(acc.count);
		for (size_t i = 0; i < acc.count; i++)
		{
			const unsigned char* p = base + i * stride;
			switch (acc.componentType)
			{
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:  out[i] = *reinterpret_cast<const uint8_t*>(p); break;
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: out[i] = *reinterpret_cast<const uint16_t*>(p); break;
			default:                                      out[i] = *reinterpret_cast<const uint32_t*>(p); break;
			}
		}
		return out;
	}

	void ComputeNormals(MeshData& data)
	{
		for (Vertex& v : data.Vertices) v.Normal = glm::vec3(0.0f);
		for (size_t i = 0; i + 2 < data.Indices.size(); i += 3)
		{
			Vertex& a = data.Vertices[data.Indices[i]];
			Vertex& b = data.Vertices[data.Indices[i + 1]];
			Vertex& c = data.Vertices[data.Indices[i + 2]];
			glm::vec3 n = glm::cross(b.Position - a.Position, c.Position - a.Position);
			a.Normal += n; b.Normal += n; c.Normal += n;
		}
		for (Vertex& v : data.Vertices)
		{
			float len = glm::length(v.Normal);
			v.Normal = len > 1e-8f ? v.Normal / len : glm::vec3(0.0f, 1.0f, 0.0f);
		}
	}

	std::optional<MeshData> ReadPrimitive(const tinygltf::Model& model, const tinygltf::Primitive& prim)
	{
		auto posIt = prim.attributes.find("POSITION");
		if (posIt == prim.attributes.end()) return std::nullopt;

		int comps = 0;
		std::vector<float> positions = ReadFloats(model, posIt->second, comps);
		if (comps != 3 || positions.empty()) return std::nullopt;
		size_t count = positions.size() / 3;

		MeshData data;
		data.Vertices.resize(count);
		for (size_t i = 0; i < count; i++)
			data.Vertices[i].Position = glm::vec3(positions[i * 3], positions[i * 3 + 1], positions[i * 3 + 2]);

		bool hasNormals = false;
		auto nrmIt = prim.attributes.find("NORMAL");
		if (nrmIt != prim.attributes.end())
		{
			std::vector<float> normals = ReadFloats(model, nrmIt->second, comps);
			if (comps == 3 && normals.size() == count * 3)
			{
				for (size_t i = 0; i < count; i++)
					data.Vertices[i].Normal = glm::vec3(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]);
				hasNormals = true;
			}
		}

		auto uvIt = prim.attributes.find("TEXCOORD_0");
		if (uvIt != prim.attributes.end())
		{
			std::vector<float> uvs = ReadFloats(model, uvIt->second, comps);
			if (comps == 2 && uvs.size() == count * 2)
				for (size_t i = 0; i < count; i++)
					data.Vertices[i].UV = glm::vec2(uvs[i * 2], uvs[i * 2 + 1]);
		}

		if (prim.indices >= 0)
		{
			data.Indices = ReadIndices(model, prim.indices);
		}
		else
		{
			data.Indices.resize(count);
			for (size_t i = 0; i < count; i++) data.Indices[i] = static_cast<uint32_t>(i);
		}

		if (!hasNormals) ComputeNormals(data);
		return data;
	}

	std::shared_ptr<Material> ReadMaterial(const tinygltf::Model& model, int materialIndex, std::vector<std::shared_ptr<TextureGL>>& textures)
	{
		auto mat = std::make_shared<Material>();
		if (materialIndex < 0 || materialIndex >= static_cast<int>(model.materials.size())) return mat;

		const tinygltf::Material& src = model.materials[materialIndex];
		const auto& pbr = src.pbrMetallicRoughness;
		if (pbr.baseColorFactor.size() == 4)
			mat->BaseColor = glm::vec4(
				static_cast<float>(pbr.baseColorFactor[0]), static_cast<float>(pbr.baseColorFactor[1]),
				static_cast<float>(pbr.baseColorFactor[2]), static_cast<float>(pbr.baseColorFactor[3]));
		mat->DoubleSided = src.doubleSided;

		int texIndex = pbr.baseColorTexture.index;
		if (texIndex >= 0 && texIndex < static_cast<int>(model.textures.size()))
		{
			int imgIndex = model.textures[texIndex].source;
			if (imgIndex >= 0 && imgIndex < static_cast<int>(model.images.size()))
			{
				if (!textures[imgIndex])
				{
					const tinygltf::Image& img = model.images[imgIndex];
					if (!img.image.empty() && img.bits == 8)
						textures[imgIndex] = TextureGL::FromPixels(img.width, img.height, img.component, img.image.data());
				}
				mat->BaseColorTexture = textures[imgIndex];
			}
		}
		return mat;
	}

	glm::mat4 NodeLocal(const tinygltf::Node& node)
	{
		if (node.matrix.size() == 16)
		{
			glm::mat4 m;
			for (int i = 0; i < 16; i++) glm::value_ptr(m)[i] = static_cast<float>(node.matrix[i]);
			return m;
		}

		glm::mat4 m(1.0f);
		if (node.translation.size() == 3)
			m = glm::translate(m, glm::vec3(static_cast<float>(node.translation[0]), static_cast<float>(node.translation[1]), static_cast<float>(node.translation[2])));
		if (node.rotation.size() == 4)
		{
			// glTF stores x, y, z, w
			glm::quat q(static_cast<float>(node.rotation[3]), static_cast<float>(node.rotation[0]), static_cast<float>(node.rotation[1]), static_cast<float>(node.rotation[2]));
			m = m * glm::mat4_cast(q);
		}
		if (node.scale.size() == 3)
			m = glm::scale(m, glm::vec3(static_cast<float>(node.scale[0]), static_cast<float>(node.scale[1]), static_cast<float>(node.scale[2])));
		return m;
	}
}

std::optional<GltfModel> GltfLoader::Load(const std::string& path)
{
	tinygltf::TinyGLTF loader;
	tinygltf::Model model;
	std::string err, warn;

	bool binary = path.size() >= 4 && (path.compare(path.size() - 4, 4, ".glb") == 0 || path.compare(path.size() - 4, 4, ".GLB") == 0);
	bool ok = binary ? loader.LoadBinaryFromFile(&model, &err, &warn, path)
	                 : loader.LoadASCIIFromFile(&model, &err, &warn, path);

	if (!warn.empty()) printf("GltfLoader: %s: %s\n", path.c_str(), warn.c_str());
	if (!ok)
	{
		printf("GltfLoader: failed to load %s: %s\n", path.c_str(), err.c_str());
		return std::nullopt;
	}

	GltfModel out;
	std::vector<std::shared_ptr<TextureGL>> textures(model.images.size());

	for (const tinygltf::Mesh& mesh : model.meshes)
	{
		std::shared_ptr<Mesh> gpuMesh;
		std::shared_ptr<Material> material;
		for (const tinygltf::Primitive& prim : mesh.primitives)
		{
			if (prim.mode != TINYGLTF_MODE_TRIANGLES && prim.mode != -1) continue;
			std::optional<MeshData> data = ReadPrimitive(model, prim);
			if (!data) continue;
			gpuMesh = std::make_shared<Mesh>(*data);
			material = ReadMaterial(model, prim.material, textures);
			break;
		}
		out.Meshes.push_back(gpuMesh);
		out.Materials.push_back(material ? material : std::make_shared<Material>());
	}

	int sceneIndex = model.defaultScene >= 0 ? model.defaultScene : (model.scenes.empty() ? -1 : 0);
	if (sceneIndex >= 0)
	{
		std::function<void(int, const glm::mat4&)> visit = [&](int nodeIndex, const glm::mat4& parent)
		{
			if (nodeIndex < 0 || nodeIndex >= static_cast<int>(model.nodes.size())) return;
			const tinygltf::Node& node = model.nodes[nodeIndex];
			glm::mat4 world = parent * NodeLocal(node);
			if (node.mesh >= 0 && node.mesh < static_cast<int>(out.Meshes.size()) && out.Meshes[node.mesh])
				out.Instances.push_back({ node.mesh, world });
			for (int child : node.children) visit(child, world);
		};
		for (int root : model.scenes[sceneIndex].nodes) visit(root, glm::mat4(1.0f));
	}

	return out;
}
