#pragma once

#include <vector>
#include <future>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include <glm/gtc/noise.hpp>

#include "Mesh.h"

struct HeightmapParams
{
	int octaves = 2;
	float persistence = 0.2f;
	float lacunarity = 0.086f;

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

	struct BlockJob
	{
		struct Verti { VertexInfo v; int i; };
		int rowStart = 0;
		int rowEnd = 0;
		int colSize = 0;
		int index = 0;
		std::vector<Verti> vertinfo;
	};

	Terrain(float tileSize, float width, float length, float height);
	~Terrain() = default;

	void SetTerrainSize(const glm::vec2& size);
	glm::vec2& GetPlaneSize() { return m_planeSize; }

	void SetHeightMapParams(HeightmapParams params);
	HeightmapParams GetHeightMapParams() { return m_heightmapParams; }

	void GenerateMesh();

	void GenerateTerrainBlock(int startRow, int endRow, int column, std::vector<VertexInfo>& vertices);
	void GenerateTerrainBlock(BlockJob& job, std::vector<VertexInfo>& vertices);

	void CalculateNormals(const std::vector<unsigned int>& indices, std::vector<VertexInfo>& inOutVertices) const;
	void UpdateHeightMap();

	Mesh& GetMesh() { return m_mesh; }

	void SetHeightSize(float size) { m_heightSize = size; }
	float GetHeightSize() { return m_heightSize; }

private:
	glm::vec3 CalculateNormalFromIndices(const std::vector<VertexInfo>& vertices, int a, int b, int c) const;
	float GetPerlinNoise(const glm::vec2& pos) const;

private:
	float m_tileSize;
	glm::vec2 m_planeSize;
	float m_heightSize;
	HeightmapParams m_heightmapParams;

	Mesh m_mesh;

	std::mutex m_mutex;
	std::condition_variable m_conditionVar;
};
