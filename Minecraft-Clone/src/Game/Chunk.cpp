#include "Chunk.h"

void Chunk::OnDestroy()
{
}

void Chunk::Generate(ChunkData* data, uint32_t seed)
{
	data->chunkCoord;
	for (int i = 0; i < Width; i++)
	{
		for (int j = 0; j < Height; j++)
		{
			for (int k = 0; k < Width; k++)
				data->blocks[i][j][k] = (j < 10 ? Block::Dirt : Block::None);
		}
	}
}

static inline void AddFaceToVertices(std::vector<Vertex>& vertices, uint32_t x, uint32_t y, uint32_t z, Face face)
{
	switch (face)
	{
	case Face::PosX:
		vertices.emplace_back(glm::vec3{ x + 1, y, z }, glm::vec3{ 1, 0, 0 }, glm::vec2{ 0, 0 });
		vertices.emplace_back(glm::vec3{ x + 1, y + 1, z }, glm::vec3{ 1, 0, 0 }, glm::vec2{ 1, 0 });
		vertices.emplace_back(glm::vec3{ x + 1, y + 1, z + 1 }, glm::vec3{ 1, 0, 0 }, glm::vec2{ 1, 1 });
		vertices.emplace_back(glm::vec3{ x + 1, y, z + 1 }, glm::vec3{ 1, 0, 0 }, glm::vec2{ 0, 1 });
		break;
	case Face::NegX:
		vertices.emplace_back(glm::vec3{ x, y, z }, glm::vec3{ -1, 0, 0 }, glm::vec2{ 0, 0 });
		vertices.emplace_back(glm::vec3{ x, y, z + 1 }, glm::vec3{ -1, 0, 0 }, glm::vec2{ 1, 0 });
		vertices.emplace_back(glm::vec3{ x, y + 1, z + 1 }, glm::vec3{ -1, 0, 0 }, glm::vec2{ 1, 1 });
		vertices.emplace_back(glm::vec3{ x, y + 1, z }, glm::vec3{ -1, 0, 0 }, glm::vec2{ 0, 1 });
		break;
	case Face::PosY:
		vertices.emplace_back(glm::vec3{ x, y + 1, z }, glm::vec3{ 0, 1, 0 }, glm::vec2{ 0, 0 });
		vertices.emplace_back(glm::vec3{ x, y + 1, z + 1 }, glm::vec3{ 0, 1, 0 }, glm::vec2{ 1, 0 });
		vertices.emplace_back(glm::vec3{ x + 1, y + 1, z + 1 }, glm::vec3{ 0, 1, 0 }, glm::vec2{ 1, 1 });
		vertices.emplace_back(glm::vec3{ x + 1, y + 1, z }, glm::vec3{ 0, 1, 0 }, glm::vec2{ 0, 1 });
		break;
	case Face::NegY:
		vertices.emplace_back(glm::vec3{ x, y, z }, glm::vec3{ 0, -1, 0 }, glm::vec2{ 0, 0 });
		vertices.emplace_back(glm::vec3{ x + 1, y, z }, glm::vec3{ 0, -1, 0 }, glm::vec2{ 1, 0 });
		vertices.emplace_back(glm::vec3{ x + 1, y, z + 1 }, glm::vec3{ 0, -1, 0 }, glm::vec2{ 1, 1 });
		vertices.emplace_back(glm::vec3{ x, y, z + 1 }, glm::vec3{ 0, -1, 0 }, glm::vec2{ 0, 1 });
		break;
	case Face::PosZ:
		vertices.emplace_back(glm::vec3{ x, y, z + 1 }, glm::vec3{ 0, 0, 1 }, glm::vec2{ 0, 0 });
		vertices.emplace_back(glm::vec3{ x + 1, y, z + 1 }, glm::vec3{ 0, 0, 1 }, glm::vec2{ 1, 0 });
		vertices.emplace_back(glm::vec3{ x + 1, y + 1, z + 1 }, glm::vec3{ 0, 0, 1 }, glm::vec2{ 1, 1 });
		vertices.emplace_back(glm::vec3{ x, y + 1, z + 1 }, glm::vec3{ 0, 0, 1 }, glm::vec2{ 0, 1 });
		break;
	case Face::NegZ:
		vertices.emplace_back(glm::vec3{ x, y, z }, glm::vec3{ 0, 0, -1 }, glm::vec2{ 0, 0 });
		vertices.emplace_back(glm::vec3{ x, y + 1, z }, glm::vec3{ 0, 0, -1 }, glm::vec2{ 1, 0 });
		vertices.emplace_back(glm::vec3{ x + 1, y + 1, z }, glm::vec3{ 0, 0, -1 }, glm::vec2{ 1, 1 });
		vertices.emplace_back(glm::vec3{ x + 1, y, z }, glm::vec3{ 0, 0, -1 }, glm::vec2{ 0, 1 });
		break;
	}
}

void Chunk::BuildMesh(ChunkData* data)
{
	auto& blocks = data->blocks;
	auto& vertices = data->vertices;
	vertices.clear();

	for (int i = 0; i < Width; i++)
	{
		for (int j = 0; j < Height; j++)
		{
			for (int k = 0; k < Width; k++)
			{
				if (blocks[i][j][k] != Block::None)
				{
					if ((i == Width - 1 && data->posX->blocks[0][j][k] == Block::None) || (i != Width - 1 && blocks[i + 1][j][k] == Block::None))
						AddFaceToVertices(vertices, i, j, k, Face::PosX);
					if ((i == 0 && data->negX->blocks[Width - 1][j][k] == Block::None) || (i != 0 && blocks[i - 1][j][k] == Block::None))
						AddFaceToVertices(vertices, i, j, k, Face::NegX);
					if (j == Height - 1 || blocks[i][j + 1][k] == Block::None)
						AddFaceToVertices(vertices, i, j, k, Face::PosY);
					if (j == 0 || blocks[i][j - 1][k] == Block::None)
						AddFaceToVertices(vertices, i, j, k, Face::NegY);
					if ((k == Width - 1 && data->posZ->blocks[i][j][0] == Block::None) || (k != Width - 1 && blocks[i][j][k + 1] == Block::None))
						AddFaceToVertices(vertices, i, j, k, Face::PosZ);
					if ((k == 0 && data->negZ->blocks[i][j][Width - 1] == Block::None) || (k != 0 && blocks[i][j][k - 1] == Block::None))
						AddFaceToVertices(vertices, i, j, k, Face::NegZ);
				}
			}
		}
	}

	auto& indices = data->indices;
	indices.clear();

	for (int i = 0; i < vertices.size(); i += 4)
	{
		indices.emplace_back(i);
		indices.emplace_back(i + 1);
		indices.emplace_back(i + 2);
		indices.emplace_back(i + 2);
		indices.emplace_back(i + 3);
		indices.emplace_back(i);
	}


}