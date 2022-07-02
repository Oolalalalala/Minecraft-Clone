#pragma once

#include <Olala.h>
#include <queue>
#include <future>
#include "Chunk.h"
using namespace Olala;

namespace std {
	template<>
	struct hash<glm::ivec2>
	{
		std::size_t operator()(const glm::ivec2& key) const
		{
			return hash<int>()(key.x) ^ hash<int>()(key.y);
		}
	};
}

// Runing on chunkGenerationThread
static void GenerateChunks(Ref<Tunnel<ChunkData*, ChunkData*>> tunnel, uint32_t seed = 0)
{
	while (true)
	{
		ChunkData* data = tunnel->PopLaneA();
		if (data == nullptr) // exit code
			break;
		Chunk::Generate(data, seed);
		tunnel->PushLaneB(data);
	}
}

static void BuildChunkMeshes(Ref<Tunnel<ChunkData*, ChunkData*>> tunnel)
{
	while (true)
	{
		ChunkData* data = tunnel->PopLaneA();
		if (data == nullptr) // exit code
			break;
		Chunk::BuildMesh(data);
		tunnel->PushLaneB(data);
	}
}

class ChunkManager : public Script
{
	int renderDistance = 8;
	std::unordered_map<glm::ivec2, Entity> chunks;
	Entity followingEntity;
	glm::ivec2 prevChunkCoord;
	
	ChunkData* chunkDataArray = nullptr;
	std::unordered_set<uint32_t> availableSlots;
	
	Ref<Tunnel<ChunkData*, ChunkData*>> chunkGenerationTunnel;
	Ref<Tunnel<ChunkData*, ChunkData*>> meshBuildingTunnel;
	std::queue<glm::ivec2> chunkGenerationQueue;
	std::queue<glm::ivec2> meshBuildingQueue;

	Ref<std::future<void>> chunkGenerationFuture;
	Ref<std::future<void>> meshBuildingFuture;


public:
	void SetFollowingEntity(Entity entity)
	{
		followingEntity = entity;
		auto& transform = entity.GetComponent<TransformComponent>();
		prevChunkCoord = ToChunkCoord(transform.Position.x, transform.Position.z);
	}

	void SetRenderDistance(uint32_t distance)
	{
		//auto CalcArraySize = [](uint32_t dis) { return 4 * dis * dis + 4 * dis + 1 + 100/*Spare*/; };
		//ChunkData* resizedArray = new ChunkData[CalcArraySize(distance)];
		//int idx = 0;
		//for (int i = 0; i < CalcArraySize(renderDistance); i++)
		//{
		//
		//}
		OLA_ERROR("Not implemented");
	}

	void StartUp()
	{
		auto& transform = followingEntity.GetComponent<TransformComponent>();
		glm::ivec2 center = ToChunkCoord(transform.Position.x, transform.Position.z);
		chunkGenerationQueue.push(center);
		for (int dis = 1; dis <= renderDistance + 1; dis++)
		{
			int minX = center.x - dis, maxX = center.x + dis, minZ = center.y - dis, maxZ = center.y + dis;
			for (int i = minX; i < maxX; i++)
				chunkGenerationQueue.push({ i, maxZ });
			for (int j = maxZ; j > minZ; j--)
				chunkGenerationQueue.push({ maxX, j });
			for (int i = maxX; i > minX; i--)
				chunkGenerationQueue.push({ i, minZ });
			for (int j = minZ; j < maxZ; j++)
				chunkGenerationQueue.push({ minX, j });
		}


		meshBuildingQueue.push(center);
		for (int dis = 1; dis <= renderDistance; dis++)
		{
			int minX = center.x - dis, maxX = center.x + dis, minZ = center.y - dis, maxZ = center.y + dis;
			for (int i = minX; i < maxX; i++)
				meshBuildingQueue.push({ i, maxZ });
			for (int j = maxZ; j > minZ; j--)
				meshBuildingQueue.push({ maxX, j });
			for (int i = maxX; i > minX; i--)
				meshBuildingQueue.push({ i, minZ });
			for (int j = minZ; j < maxZ; j++)
				meshBuildingQueue.push({ minX, j });
		}
	}


	static glm::ivec2 ToChunkCoord(float x, float z)
	{
		constexpr float scaler = 1.f / Chunk::Width;
		return { std::floor(x * scaler), std::floor(z * scaler) };
	}

	bool InRange(const glm::ivec2& coord, int distance)
	{
		return std::labs(coord.x - prevChunkCoord.x) <= distance && std::labs(coord.y - prevChunkCoord.y) <= distance;
	}

	bool InRange(const glm::ivec2& center, const glm::ivec2& coord, int distance)
	{
		return std::labs(coord.x - center.x) <= distance && std::labs(coord.y - center.y) <= distance;
	}

	static void Temp()
	{

	}

	void OnCreate()
	{
		int arraySize = CalculateChunkDataArraySize(renderDistance);
		chunkDataArray = new ChunkData[arraySize];
		for (int i = 0; i < arraySize; i++)
			availableSlots.insert(i);

		chunkGenerationTunnel = CreateRef<Tunnel<ChunkData*, ChunkData*>>();
		meshBuildingTunnel = CreateRef<Tunnel<ChunkData*, ChunkData*>>();

		chunkGenerationFuture = CreateRef<std::future<void>>(std::async(GenerateChunks, chunkGenerationTunnel, 0));
		meshBuildingFuture = CreateRef<std::future<void>>(std::async(BuildChunkMeshes, meshBuildingTunnel));
	}

	void MaintainChunksToSurround()
	{
		// Adding chunks
		glm::vec3 position = followingEntity.GetComponent<TransformComponent>().Position;
		glm::ivec2 currentCoord = ToChunkCoord(position.x, position.z);
		if (currentCoord != prevChunkCoord)
		{
			for (int x = currentCoord.x - renderDistance - 1; x <= currentCoord.x + renderDistance + 1; x++)
			{
				for (int z = currentCoord.y - renderDistance - 1; z <= currentCoord.y + renderDistance + 1; z++)
				{
					if (!InRange(prevChunkCoord, { x, z }, renderDistance + 1))
						chunkGenerationQueue.push({ x, z });
					if (InRange(currentCoord, { x, z }, renderDistance) && !InRange(prevChunkCoord, { x, z }, renderDistance))
						meshBuildingQueue.push({ x, z });
				}
			}
			prevChunkCoord = currentCoord;
		}


		// Remove chunks that are out of range
		for (auto it = chunks.begin(); it != chunks.end(); it++)
		{
			ChunkData* data = it->second.GetComponent<Chunk>().chunkData;
			if (!InRange(data->chunkCoord, renderDistance + 1))
			{
				if (ReadyToBeRemoved(data))
				{
					Destroy(it->second);
					it = chunks.erase(it);
					if (it == chunks.end())
						break;

					if (data->posX) data->posX->negX = nullptr;
					if (data->negX) data->negX->posX = nullptr;
					if (data->posZ) data->posZ->negZ = nullptr;
					if (data->negZ) data->negZ->posZ = nullptr;
					data->posX = data->negX = data->posZ = data->negZ = nullptr;
					data->status = ChunkData::Status::Tombstone;
					availableSlots.insert(data - chunkDataArray);
				}
			}
		}
	}

	void ProcessChunkGeneration()
	{
		// Send data
		while (chunkGenerationQueue.size() && availableSlots.size())
		{
			if (chunks.find(chunkGenerationQueue.front()) != chunks.end() || !InRange(chunkGenerationQueue.front(), renderDistance + 1))
			{
				chunkGenerationQueue.pop();
				continue;
			}

			ChunkData* data = chunkDataArray + *availableSlots.begin();
			data->chunkCoord = chunkGenerationQueue.front();
			data->status = ChunkData::Status::Generating;
			chunkGenerationTunnel->PushLaneA(data);
			chunkGenerationQueue.pop();
			availableSlots.erase(availableSlots.begin());
		}

		const int maxRetrievePerFrame = 3;
		int count = 0;

		// Retrieve data
		while (chunkGenerationTunnel->LaneBSize())
		{
			ChunkData* data = chunkGenerationTunnel->PopLaneB();
			Entity chunk = Instantiate("Chunk : " + std::to_string(data->chunkCoord.x) + ":" + std::to_string(data->chunkCoord.y));
			chunk.GetComponent<TransformComponent>().Position = { data->chunkCoord.x * Chunk::Width, 0, data->chunkCoord.y * Chunk::Width };
			chunk.AddComponent<Chunk>().chunkData = data;
			data->status = ChunkData::Status::Generated;
			if (chunks.find({ data->chunkCoord.x + 1, data->chunkCoord.y }) != chunks.end())
			{
				ChunkData* dataPosX = chunks[{data->chunkCoord.x + 1, data->chunkCoord.y}].GetComponent<Chunk>().chunkData;
				data->posX = dataPosX;
				dataPosX->negX = data;
			}
			if (chunks.find({ data->chunkCoord.x - 1, data->chunkCoord.y }) != chunks.end())
			{
				ChunkData* dataNegX = chunks[{data->chunkCoord.x - 1, data->chunkCoord.y}].GetComponent<Chunk>().chunkData;
				data->negX = dataNegX;
				dataNegX->posX = data;
			}
			if (chunks.find({ data->chunkCoord.x, data->chunkCoord.y + 1 }) != chunks.end())
			{
				ChunkData* dataPosZ = chunks[{data->chunkCoord.x, data->chunkCoord.y + 1}].GetComponent<Chunk>().chunkData;
				data->posZ = dataPosZ;
				dataPosZ->negZ = data;
			}
			if (chunks.find({ data->chunkCoord.x, data->chunkCoord.y - 1 }) != chunks.end())
			{
				ChunkData* dataNegZ = chunks[{data->chunkCoord.x, data->chunkCoord.y - 1}].GetComponent<Chunk>().chunkData;
				data->negZ = dataNegZ;
				dataNegZ->posZ = data;
			}
			chunks[data->chunkCoord] = chunk;

			if (++count > maxRetrievePerFrame)
				break;
		}
	}

	void ProcessMeshBuilding()
	{
		// Send data
		while (meshBuildingQueue.size())
		{
			if (!InRange(meshBuildingQueue.front(), renderDistance))
			{
				meshBuildingQueue.pop();
				continue;
			}
			if (chunks.find(meshBuildingQueue.front()) == chunks.end())
				break;
			ChunkData* data = chunks[meshBuildingQueue.front()].GetComponent<Chunk>().chunkData;
			if (data->status == ChunkData::Status::Generated && data->posX && data->negX && data->posZ && data->negZ)
			{
				data->status = ChunkData::Status::BuildingMesh;
				meshBuildingTunnel->PushLaneA(data);
				meshBuildingQueue.pop();
			}
			else
				break;
		}


		static Ref<Material> material = CreateRef<Material>(Texture2D::Create("../Olala/Asset/Texture/orange_cross.png"));

		const int maxRetrievePerFrame = 3;
		int count = 0;

		// Retrieve data
		while (meshBuildingTunnel->LaneBSize())
		{
			ChunkData* data = meshBuildingTunnel->PopLaneB();
			Entity chunk = chunks[data->chunkCoord];
			chunk.AddComponent<MeshRendererComponent>(Mesh::Create(data->vertices, data->indices), material);
			data->vertices.clear();
			data->indices.clear();
			data->status = ChunkData::Status::Complete;

			if (++count > maxRetrievePerFrame)
				break;
		}
	}

	void OnDestroy() override
	{
		// End async
		chunkGenerationTunnel->PushLaneA(nullptr);
		meshBuildingTunnel->PushLaneA(nullptr);

		delete[] chunkDataArray;
	}

	static inline bool ReadyToBeRemoved(ChunkData* data)
	{
		if (data->status == ChunkData::Status::Generating || data->status == ChunkData::Status::BuildingMesh)
			return false;
		if ((data->posX && data->posX->status == ChunkData::Status::BuildingMesh) ||
			(data->negX && data->negX->status == ChunkData::Status::BuildingMesh) ||
			(data->posZ && data->posZ->status == ChunkData::Status::BuildingMesh) ||
			(data->negZ && data->negZ->status == ChunkData::Status::BuildingMesh))
			return false;
		return true;
	}

	static uint32_t CalculateChunkDataArraySize(uint32_t renderDistance)
	{
		return 4 * renderDistance * renderDistance + 6 * renderDistance + 9 + 150/*Spare*/;
	}

};


class ChunkManagerSystem : public ComponentSystem
{
	void OnUpdate(float dt) override
	{
		for (auto [e, manager] : GetEntities<ChunkManager>())
		{
			manager.MaintainChunksToSurround();
			manager.ProcessChunkGeneration();
			manager.ProcessMeshBuilding();
		}
	}
};