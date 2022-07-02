#pragma once

#include <Olala.h>
using namespace Olala;

enum class Block : unsigned char
{
	None = 0, Dirt, Grass, Wood
};

enum class Face : unsigned char
{
	PosX = 0, NegX, PosY, NegY, PosZ, NegZ
};

struct ChunkData;

class Chunk : public Script
{
public:
	static const int Width = 8, Height = 256;

	ChunkData* chunkData = nullptr;

private:
	void OnDestroy() override;


public:

	static void Generate(ChunkData* data, uint32_t seed = 0);
	static void BuildMesh(ChunkData* data);

	inline Block GetBlock(uint32_t x, uint32_t y, uint32_t z);
	inline void SetBlock(uint32_t x, uint32_t y, uint32_t z, Block block);

};

const int Chunk::Width, Chunk::Height;

struct ChunkData
{
	enum class Status : unsigned char
	{
		Tombstone = 0, Generating, Generated, BuildingMesh, Complete
	};

	Status status = Status::Tombstone;
	glm::ivec2 chunkCoord;
	ChunkData* posX = nullptr, * negX = nullptr, * posZ = nullptr, * negZ = nullptr;
	Block blocks[Chunk::Width][Chunk::Height][Chunk::Width];

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};


inline Block Chunk::GetBlock(uint32_t x, uint32_t y, uint32_t z)
{
	return chunkData->blocks[x][y][z];
}

inline void Chunk::SetBlock(uint32_t x, uint32_t y, uint32_t z, Block block)
{
	chunkData->blocks[x][y][z] = block;
}
