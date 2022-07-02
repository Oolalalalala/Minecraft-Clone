#include "pch.h"
#include "RiggedMesh.h"

namespace Olala {
	
	RiggedMesh::RiggedMesh(const std::vector<WeightedVertex>& vertices, const std::vector<uint32_t>& indices)
	{

	}

	RiggedMesh::~RiggedMesh()
	{
	}

	Ref<RiggedMesh> RiggedMesh::Create(const std::vector<WeightedVertex>& vertices, const std::vector<uint32_t>& indices)
	{
		return CreateRef<RiggedMesh>(vertices, indices);
	}

}