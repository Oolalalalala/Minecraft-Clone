#include "pch.h"
#include "ColladaParser.h"

#include "pugixml/pugixml.hpp"
#include <iostream>

namespace Olala {

	static inline pugi::xml_node GetDataNode(pugi::xml_node node)
	{
		while (node.attribute("source"))
		{
			std::string src(node.attribute("source").value());
			src.erase(src.begin()); // Remove the # in the front
			node = node.parent().parent().find_child_by_attribute("id", src.c_str()).first_child();
		}
		return node;
	}

	template<typename T>
	static inline void StringToArray(const std::string& str, std::vector<T>& values, int count)
	{
		std::stringstream ss(str);
		values.resize(count);
		for (int i = 0; i < count; i++)
			ss >> values[i];
	}

	static inline glm::vec4 StringToVec4(const std::string& str)
	{
		std::stringstream ss(str);
		glm::vec4 v;
		ss >> v.x >> v.y >> v.z >> v.w;
		return v;
	}


	struct EffectData
	{
		glm::vec4 Emmision = glm::vec4(0.0, 0.0, 0.0, 1.0);
		Ref<Texture2D> EmmisionTexture = nullptr;
		glm::vec4 Ambient = glm::vec4(0.0, 0.0, 0.0, 1.0);
		Ref<Texture2D> AmbientTexture = nullptr;
		glm::vec4 Diffuse = glm::vec4(0.0, 0.0, 0.0, 1.0);
		Ref<Texture2D> DiffuseTexture = nullptr;
		float Shinnyness = 0.f;
		glm::vec4 Reflective = glm::vec4(0.0, 0.0, 0.0, 1.0);
		float Reflectivity = 0.f;
		glm::vec4 Transparent = glm::vec4(0.0, 0.0, 0.0, 1.0);
		float Transparency = 0.f;
		float IndexOfRefraction = 1.f;
	};

	struct ModelData
	{
		std::vector<Vertex> Vertices;
		std::vector<uint32_t> Indices;
		std::vector<glm::vec3> JointIDsArray;
		std::vector<glm::vec3> JointWeightsArray;
		Ref<Material> Material;
		bool IsRigged = false;
	};


	Ref<Model> ColladaParser::Parse(const std::filesystem::path& path)
	{
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file(path.wstring().c_str());

		if (!result)
		{
			OLA_CORE_ERROR("Failed to load Collada file, path = {0}", path);
			return nullptr;
		}


		std::unordered_map<std::string, std::string> imagePaths;
		std::unordered_map<std::string, EffectData> effects;
		std::unordered_map<std::string, Ref<Material>> materials;
		std::unordered_map<std::string, ModelData> modelDatas;

		auto collada = doc.child("COLLADA");

		if (auto imageLib = collada.child("library_images"))
		{
			for (auto image : imageLib)
			{
				std::string imgName = image.child("init_from").child_value();
				while (true)
				{
					size_t pos = imgName.find("%20");
					if (pos == std::string::npos) break;
					imgName.replace(pos, 3, " ");
				}
				auto imgPath = path.parent_path() / std::filesystem::path(imgName);

				if (auto id = image.attribute("id"))
					imagePaths[id.value()] = imgPath.string();
			}
		}

		if (auto effectLib = collada.child("library_effects"))
		{
			for (auto effect : effectLib)
			{
				std::unordered_map<std::string, std::string> surfaces;
				std::unordered_map<std::string, Ref<Texture2D>> sampler2Ds;

				for (auto param : effect.child("profile_COMMON").children("newparam"))
				{
					if (auto surface = param.child("surface"))
					{
						if (auto init = surface.child("init_from"))
							surfaces[param.attribute("sid").value()] = imagePaths[init.child_value()];
					}
					if (auto sampler2D = param.child("sampler2D"))
					{
						// Get wrap and filter data here

						if (auto source = sampler2D.child("source"))
							sampler2Ds[param.attribute("sid").value()] = Texture2D::Create(surfaces[source.child_value()]);
					}
				}

				auto& effectData = effects[effect.attribute("id").value()];

				auto shading = effect.child("profile_COMMON").child("technique").first_child();
				if (auto emission = shading.child("emission"))
				{
					if (auto color = emission.child("color"))
						effectData.Emmision = StringToVec4(color.child_value());
					if (auto texture = emission.child("texture"))
						effectData.EmmisionTexture = sampler2Ds[texture.attribute("texture").value()];
				}
				if (auto ambient = shading.child("ambient"))
				{
					if (auto color = ambient.child("color"))
						effectData.Ambient = StringToVec4(color.child_value());
					if (auto texture = ambient.child("texture"))
						effectData.AmbientTexture = sampler2Ds[texture.attribute("texture").value()];
				}
				if (auto diffuse = shading.child("diffuse"))
				{
					if (auto color = diffuse.child("color"))
						effectData.Diffuse = StringToVec4(color.child_value());
					if (auto texture = diffuse.child("texture"))
						effectData.DiffuseTexture = sampler2Ds[texture.attribute("texture").value()];
				}

			}
		}

		if (auto materialLib = collada.child("library_materials"))
		{
			for (auto material : materialLib)
			{
				if (auto effect = material.child("instance_effect"))
				{
					std::string url(effect.attribute("url").value());
					EffectData& data = effects[url.substr(1, url.size() - 1)];
					materials[material.attribute("id").value()] = Material::Create(data.DiffuseTexture, glm::vec4(1.0));
				}
			}
		}

		if (auto geometryLib = collada.child("library_geometries"))
		{
			for (auto geometry : geometryLib.children())
			{
				if (auto mesh = geometry.child("mesh"))
				{
					std::vector<float> positions, normals, texCoords;
					std::vector<uint32_t> faces, idxs;
					uint32_t vertexCount = 0, stride = 0;
					std::string materialID;


					// Polylist layout
					if (auto polyList = mesh.child("polylist"))
					{
						for (auto vertexAttribute : polyList.children("input"))
						{
							std::string semantic = vertexAttribute.attribute("semantic").value();
							if (semantic == "VERTEX")
							{
								auto dataNode = GetDataNode(vertexAttribute);
								int count = dataNode.attribute("count").as_int();
								std::string dataStr(dataNode.child_value());
								StringToArray(dataStr, positions, count);
							}
							else if (semantic == "NORMAL")
							{
								auto dataNode = GetDataNode(vertexAttribute);
								int count = dataNode.attribute("count").as_int();
								std::string dataStr(dataNode.child_value());
								StringToArray(dataStr, normals, count);
							}
							else if (semantic == "TEXCOORD")
							{
								auto dataNode = GetDataNode(vertexAttribute);
								int count = dataNode.attribute("count").as_int();
								std::string dataStr(dataNode.child_value());
								StringToArray(dataStr, texCoords, count);
							}
							else if (semantic == "COLOR")
							{
								OLA_CORE_WARN("Vertex coloring is not supported");
							}
							else
							{
								OLA_CORE_WARN("Unknown vertex attribute {0}", semantic);
							}

							stride++;
						}

						uint32_t polygonCount = polyList.attribute("count").as_uint();

						if (auto polygons = polyList.child("vcount"))
						{
							StringToArray(polygons.child_value(), faces, polygonCount);
							for (uint32_t& face : faces)
								vertexCount += face;
						}

						if (auto p = polyList.child("p"))
						{
							int count = vertexCount * stride;
							std::string dataStr(p.child_value());
							StringToArray(dataStr, idxs, count);
						}

						// Material
						materialID = polyList.attribute("material").value();
					}


					// Triangle layout
					if (auto triangles = mesh.child("triangles"))
					{
						for (auto vertexAttribute : triangles.children("input"))
						{
							std::string semantic = vertexAttribute.attribute("semantic").value();
							if (semantic == "VERTEX")
							{
								auto dataNode = GetDataNode(vertexAttribute);
								int count = dataNode.attribute("count").as_int();
								std::string dataStr(dataNode.child_value());
								StringToArray(dataStr, positions, count);
							}
							else if (semantic == "NORMAL")
							{
								auto dataNode = GetDataNode(vertexAttribute);
								int count = dataNode.attribute("count").as_int();
								std::string dataStr(dataNode.child_value());
								StringToArray(dataStr, normals, count);
							}
							else if (semantic == "TEXCOORD")
							{
								auto dataNode = GetDataNode(vertexAttribute);
								int count = dataNode.attribute("count").as_int();
								std::string dataStr(dataNode.child_value());
								StringToArray(dataStr, texCoords, count);
							}
							else if (semantic == "COLOR")
							{
								OLA_CORE_WARN("Vertex coloring is not supported");
							}
							else
							{
								OLA_CORE_WARN("Unknown vertex attribute {0}", semantic);
							}

							stride++;
						}

						uint32_t triangleCount = triangles.attribute("count").as_uint();
						
						faces.resize(triangleCount);
						std::fill(faces.begin(), faces.end(), 3);
						vertexCount = triangleCount * 3;

						if (auto p = triangles.child("p"))
						{
							int count = vertexCount * stride;
							std::string dataStr(p.child_value());
							StringToArray(dataStr, idxs, count);
						}

						// Material
						materialID = triangles.attribute("material").value();
					}


					// Must have position, normal, texCoord
					if (stride < 3) continue;


					// Create mesh data
					std::string geometryID = geometry.attribute("id").value();

					// Vertices
					auto& vertices = modelDatas[geometryID].Vertices;
					vertices.resize(vertexCount);
					for (int i = 0; i < vertexCount; i++)
					{
						uint32_t posIdx = idxs[i * stride], normIdx = idxs[i * stride + 1], texCoordIdx = idxs[i * stride + 2];
						vertices[i].Position = glm::vec3{ positions[posIdx * 3], positions[posIdx * 3 + 1], positions[posIdx * 3 + 2] };
						vertices[i].Normal = glm::vec3{ normals[normIdx * 3], normals[normIdx * 3 + 1], normals[normIdx * 3 + 2] };
						vertices[i].TexCoord = glm::vec2{ texCoords[texCoordIdx * 2], texCoords[texCoordIdx * 2 + 1] };
					}

					// Indices
					auto& indices = modelDatas[geometryID].Indices;
					int cnt = 0;
					for (uint32_t face : faces)
					{
						for (int i = 0; i < face - 2; i++)
						{
							indices.emplace_back(cnt);
							indices.emplace_back(cnt + i + 1);
							indices.emplace_back(cnt + i + 2);
						}
						cnt += face;
					}

					// Material
					modelDatas[geometryID].Material = materials[materialID];
				}
			}
		}

		if (auto controllerLib = collada.child("library_controllers"))
		{

		}
		
		if (auto animationLib = collada.child("library_animations"))
		{
			for (auto animation : animationLib)
			{
				
			}
		}

		if (auto visualSceneLib = collada.child("library_visual_scenes"))
		{

		}


		// Create models
		std::vector<Ref<Model>> models;
		std::vector<Ref<RiggedModel>> riggedModels;

		for (auto& [id, data] : modelDatas)
		{
			if (data.IsRigged)
			{
				// combine weights into vertices
			}
			else
			{
				auto model = Model::Create(Mesh::Create(data.Vertices, data.Indices), data.Material);
				models.emplace_back(model);
			}
		}

		// TODO : return all models
		return models[0];
	}

}