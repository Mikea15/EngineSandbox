#pragma once

#include <vector>

#include <glm/gtc/noise.hpp>

#include "Mesh.h"

struct HeightmapParams
{
	unsigned int octaves;
	float lacunarity;
	float persistence;

	bool operator!=(const HeightmapParams& a) const 
	{
		return !(*this == a);
	}

	bool operator==(const HeightmapParams& a) const
	{
		if (octaves != a.octaves) return false;
		if (lacunarity != a.lacunarity) return false;
		if (persistence != a.persistence) return false;
		return true;
	}

};

class Terrain
{
public:
	Terrain();
	~Terrain() = default;

	void SetTerrainSize(const glm::vec2& size);
	glm::vec2& GetPlaneSize() { return m_planeSize; }

	void SetHeightMapParams(HeightmapParams params);
	HeightmapParams GetHeightMapParams() { return m_heightmapParams; }

	void GenerateMesh();
	void CalculateNormals();
	void UpdateHeightMap();

	Mesh GetMesh() { return m_mesh; }

	void SetHeightSize(float size) { m_heightSize = size; }
	float GetHeightSize() { return m_heightSize; }

private:
	glm::vec3 CalculateNormalFromIndices(const std::vector<VertexInfo>& vertices, int a, int b, int c);
	float GetPerlinNoise(const glm::vec2& pos);

private:
	float m_tileSize;
	glm::vec2 m_planeSize;
	float m_heightSize;
	HeightmapParams m_heightmapParams;

	Mesh m_mesh;
};
