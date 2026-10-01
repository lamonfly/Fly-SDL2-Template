#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include "Mesh.h"
#include "TextureGL.h"
#include "GltfLoader.h"

// Scene-owned, dies before the GL context
class AssetCache
{
public:
	std::shared_ptr<Mesh> GetMesh(const std::string& key, const std::function<MeshData()>& make)
	{
		auto it = mMeshes.find(key);
		if (it != mMeshes.end()) return it->second;
		auto mesh = std::make_shared<Mesh>(make());
		mMeshes[key] = mesh;
		return mesh;
	}

	std::shared_ptr<TextureGL> GetTexture(const std::string& path)
	{
		auto it = mTextures.find(path);
		if (it != mTextures.end()) return it->second;
		auto tex = TextureGL::FromFile(path);
		if (tex) mTextures[path] = tex;
		return tex;
	}

	// Null when load failed
	const GltfModel* GetModel(const std::string& path)
	{
		auto it = mModels.find(path);
		if (it != mModels.end()) return it->second ? &*it->second : nullptr;
		auto& slot = mModels[path];
		slot = GltfLoader::Load(path);
		return slot ? &*slot : nullptr;
	}

private:
	std::unordered_map<std::string, std::shared_ptr<Mesh>> mMeshes;
	std::unordered_map<std::string, std::shared_ptr<TextureGL>> mTextures;
	std::unordered_map<std::string, std::optional<GltfModel>> mModels;
};
