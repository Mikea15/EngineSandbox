#include "Terrain.h"

Terrain::Terrain()
	: m_tileSize(1.0f)
	, m_planeSize(20.0f, 20.0f)
	, m_heightSize(1.0f)
{
	m_heightmapParams = { 2, 1.0f / 24.f, 2.3f };
}

void Terrain::SetTerrainSize(const glm::vec2& size)
{
	m_planeSize = size;
}

void Terrain::SetHeightMapParams(HeightmapParams params)
{
	m_heightmapParams = params;
	UpdateHeightMap();
}

void Terrain::GenerateMesh()
{
	int nTiles = static_cast<int>(m_planeSize.x * m_planeSize.y);
	int length = static_cast<int>(m_planeSize.x);
	int width = static_cast<int>(m_planeSize.y);
	int nVerts = nTiles * 4;
	int nTris = nTiles * 2;
	int nVertsPerTris = nTiles * 6;

	// Create mesh data
	std::vector<VertexInfo> vertices;
	vertices.resize(nVerts);

	std::vector<unsigned int> indices;
	indices.resize(nVertsPerTris);

	for (int z = 0; z < length; ++z)
	{
		for (int x = 0; x < width; ++x)
		{
			int index = (z * width + x) * 4;

			auto p = GetPerlinNoise(glm::vec2(x, z)) * m_heightSize;
			auto p1 = GetPerlinNoise(glm::vec2(x + 1, z)) * m_heightSize;
			auto p2 = GetPerlinNoise(glm::vec2(x, z + 1)) * m_heightSize;
			auto p3 = GetPerlinNoise(glm::vec2(x + 1, z + 1)) * m_heightSize;
			
			auto v0 = glm::vec3(x * m_tileSize, p, z * m_tileSize);
			auto v1 = glm::vec3(((x + 1) * m_tileSize), p1, z * m_tileSize);
			auto v2 = glm::vec3(x * m_tileSize, p2, ((z + 1) * m_tileSize));
			auto v3 = glm::vec3(((x + 1) * m_tileSize), p3, ((z + 1) * m_tileSize));

			vertices[index + 0].Position = v0;
			vertices[index + 1].Position = v1;
			vertices[index + 2].Position = v2;
			vertices[index + 3].Position = v3;

			vertices[index + 0].Normal = glm::normalize(glm::cross(v0 - v2, v0 - v3));
			vertices[index + 1].Normal = glm::normalize(glm::cross(v1 - v0, v1 - v3));
			vertices[index + 2].Normal = glm::normalize(glm::cross(v2 - v3, v2 - v0));
			vertices[index + 3].Normal = glm::normalize(glm::cross(v3 - v1, v3 - v0));

			// uvs
			vertices[index + 0].TexCoords = glm::vec2(0, 0);
			vertices[index + 1].TexCoords = glm::vec2(1, 0);
			vertices[index + 2].TexCoords = glm::vec2(0, 1);
			vertices[index + 3].TexCoords = glm::vec2(1, 1);
		}
	}

	m_mesh.SetVertices(vertices);

	for (int v = 0; v < vertices.size(); v += 4)
	{
		/* 2 --- 3
		 * | \   |
		 * |   \ |
		 * 0 --- 1
		 */
		int triIndex = v / 4 * 6;

		// triangles
		indices[triIndex + 0] = v;
		indices[triIndex + 1] = v + 2;
		indices[triIndex + 2] = v + 1;

		indices[triIndex + 3] = v + 2;
		indices[triIndex + 4] = v + 3;
		indices[triIndex + 5] = v + 1;
	}

	m_mesh.SetIndices(indices);

	CalculateNormals();

	m_mesh.CreateBuffers();
}

void Terrain::CalculateNormals()
{
	auto& vertices = m_mesh.GetVertices();
	auto& indices = m_mesh.GetIndices();
	const unsigned int indicesSize = static_cast<unsigned int>(indices.size() / 3);
	const unsigned int verticesSize = static_cast<unsigned int>(vertices.size());
	for (unsigned int t = 0; t < indicesSize; ++t)
	{
		int triangleIndex = t * 3;
		int indA = indices[triangleIndex + 0];
		int indB = indices[triangleIndex + 1];
		int indC = indices[triangleIndex + 2];

		glm::vec3 normal = CalculateNormalFromIndices(vertices, indA, indB, indC);
		vertices[indA].Normal += normal;
		vertices[indB].Normal += normal;
		vertices[indC].Normal += normal;
	}

	for (unsigned int v = 0; v < verticesSize; ++v)
	{
		vertices[v].Normal = glm::normalize(vertices[v].Normal);
	}
}

void Terrain::UpdateHeightMap()
{
	int length = static_cast<int>(m_planeSize.x);
	int width = static_cast<int>(m_planeSize.y);

	auto& vertices = m_mesh.GetVertices();

#pragma omp parallel for
	for (int z = 0; z < length; ++z)
	{
		for (int x = 0; x < width; ++x)
		{
			int index = (z * width + x) * 4;

			vertices[index + 0].Position.y = GetPerlinNoise(glm::vec2(x, z)) * m_heightSize;
			vertices[index + 1].Position.y = GetPerlinNoise(glm::vec2(x + 1, z)) * m_heightSize;
			vertices[index + 2].Position.y = GetPerlinNoise(glm::vec2(x, z + 1)) * m_heightSize;
			vertices[index + 3].Position.y = GetPerlinNoise(glm::vec2(x + 1, z + 1)) * m_heightSize;
		}
	}

	CalculateNormals();

	m_mesh.CreateBuffers();
}

glm::vec3 Terrain::CalculateNormalFromIndices(const std::vector<VertexInfo>& vertices, int a, int b, int c)
{
	glm::vec3 pA = vertices[a].Position;
	glm::vec3 pB = vertices[b].Position;
	glm::vec3 pC = vertices[c].Position;

	glm::vec3 ab = pB - pA;
	glm::vec3 ac = pC - pA;

	return glm::normalize(glm::cross(ab, ac));
}

float Terrain::GetPerlinNoise(const glm::vec2 & pos)
{
	float total = 0.0f;
	float frequency = 1.0f;
	float amplitude = 1.0f;
	float maxVal = 0.0f;

	for (unsigned int i = 0; i < m_heightmapParams.octaves; i++)
	{
		total += glm::perlin(pos * frequency) * amplitude;
		maxVal += amplitude;
		amplitude *= m_heightmapParams.persistence;
		frequency *= m_heightmapParams.lacunarity;
	}

	if (maxVal > 0.0f)
		return total / maxVal;

	return 1.0f;
}
