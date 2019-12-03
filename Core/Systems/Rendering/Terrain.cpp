#include "Terrain.h"

#define MULTITHREAD 1
#define MUTEX_WRITE 1
 

Terrain::Terrain(float tileSize, float width, float length, float height)
	: m_tileSize(tileSize)
	, m_planeSize(width, length)
	, m_heightSize(height)
{
	m_heightmapParams = {};
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
	auto startTime = std::chrono::high_resolution_clock::now();

	int nTiles = static_cast<int>(m_planeSize.x * m_planeSize.y);
	int width = static_cast<int>(m_planeSize.x);
	int length = static_cast<int>(m_planeSize.y);
	int nVerts = nTiles * 4;
	int nTris = nTiles * 2;
	int nVertsPerTris = nTiles * 6;

	// Create mesh data
	std::vector<VertexInfo> vertices;
	vertices.resize(nVerts);

	std::vector<unsigned int> indices;
	indices.resize(nVertsPerTris);

#if MULTITHREAD
	const int nThreads = std::thread::hardware_concurrency();
	int launchedThreads = nThreads - 1;
	int nRowsPerJob = std::min(5, length);
	int nJobs = length / nRowsPerJob;
	int leftOver = length % nThreads;

	std::queue<BlockJob> jobQueue;
	std::vector<BlockJob> jobsDone;
	std::vector<std::thread> threads;

	for (int i = 0; i < nJobs; ++i)
	{
		BlockJob job;
		job.rowStart = i * nRowsPerJob;
		job.rowEnd = job.rowStart + nRowsPerJob;
		if (i == nThreads - 1)
		{
			job.rowEnd = job.rowStart + nRowsPerJob + leftOver;
		}
		job.colSize = width;
		jobQueue.push(job);
	}

	std::atomic<int> finishedThreads{ 0 };
	for (int i = 0; i < launchedThreads; ++i)
	{
		std::thread t([&]() {
			bool hasWork = true;
			while (hasWork)
			{
				hasWork = false;
				BlockJob job;
				{
					std::lock_guard<std::mutex> lock(m_mutex);
					hasWork = !jobQueue.empty();
					if (hasWork)
					{
						job = jobQueue.front();
						jobQueue.pop();
					}
				}
				// quick/dirty way to find if a job is valid
				if (job.rowStart < job.rowEnd)
				{
					GenerateTerrainBlock(job, vertices);

					{
						std::lock_guard<std::mutex> lock(m_mutex);
						jobsDone.push_back(std::move(job));
					}
				}
			}

			// no more jobs.
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				finishedThreads++;
				m_conditionVar.notify_one();
			}
			});
		threads.push_back(std::move(t));
	}

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_conditionVar.wait(lock, [&finishedThreads, &launchedThreads] {
			return finishedThreads == launchedThreads;
			});
	}

	for (std::thread& t : threads)
	{
		t.join();
	}

#if !MUTEX_WRITE
	for (BlockJob job : jobsDone)
	{
		for (BlockJob::Verti verti : job.vertinfo)
		{
			vertices[verti.i] = verti.v;
		}
	}
#endif

#else 
	GenerateTerrainBlock(0, length, width, vertices);
#endif

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

	CalculateNormals(indices, vertices);

	m_mesh.SetVertices(vertices);
	m_mesh.SetIndices(indices);
	
	m_mesh.CreateBuffers();

	vertices.clear();
	indices.clear();

	auto timeSpan = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime);
	int frameTimeMs = static_cast<int>(timeSpan.count());
	std::cout << "[Terrain] Generated " << width << "x" << length << " terrain in " << frameTimeMs << " ms \n";
}

void Terrain::GenerateTerrainBlock(int startRow, int endRow, int column, std::vector<VertexInfo>& vertices)
{
	for (int z = startRow; z < endRow; ++z)
	{
		for (int x = 0; x < column; ++x)
		{
			int index = (z * column + x) * 4;

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
}

void Terrain::GenerateTerrainBlock(BlockJob& job, std::vector<VertexInfo>& vertices)
{
	for (int z = job.rowStart; z < job.rowEnd; ++z)
	{
		for (int x = 0; x < job.colSize; ++x)
		{
			job.index = (z * job.colSize + x) * 4;
			int index = job.index;

			auto p = GetPerlinNoise(glm::vec2(x, z)) * m_heightSize;
			auto p1 = GetPerlinNoise(glm::vec2(x + 1, z)) * m_heightSize;
			auto p2 = GetPerlinNoise(glm::vec2(x, z + 1)) * m_heightSize;
			auto p3 = GetPerlinNoise(glm::vec2(x + 1, z + 1)) * m_heightSize;

			auto v0 = glm::vec3(x * m_tileSize, p, z * m_tileSize);
			auto v1 = glm::vec3(((x + 1) * m_tileSize), p1, z * m_tileSize);
			auto v2 = glm::vec3(x * m_tileSize, p2, ((z + 1) * m_tileSize));
			auto v3 = glm::vec3(((x + 1) * m_tileSize), p3, ((z + 1) * m_tileSize));

#if !MUTEX_WRITE
			BlockJob::Verti vi0;
			vi0.i = index;
			vi0.v.Position = v0;
			vi0.v.Normal = glm::normalize(glm::cross(v0 - v2, v0 - v3));
			vi0.v.TexCoords = glm::vec2(0, 0);

			BlockJob::Verti vi1;
			vi1.i = index + 1;
			vi1.v.Position = v1;
			vi1.v.Normal = glm::normalize(glm::cross(v1 - v0, v1 - v3));
			vi1.v.TexCoords = glm::vec2(1, 0);

			BlockJob::Verti vi2;
			vi2.i = index + 2;
			vi2.v.Position = v2;
			vi2.v.Normal = glm::normalize(glm::cross(v2 - v3, v2 - v0));
			vi2.v.TexCoords = glm::vec2(0, 1);

			BlockJob::Verti vi3;
			vi3.i = index + 3;
			vi3.v.Position = v3;
			vi3.v.Normal = glm::normalize(glm::cross(v3 - v1, v3 - v0));
			vi3.v.TexCoords = glm::vec2(1, 1);

			job.vertinfo.push_back(vi0);
			job.vertinfo.push_back(vi1);
			job.vertinfo.push_back(vi2);
			job.vertinfo.push_back(vi3);
#else
			{
				std::lock_guard<std::mutex> lock(m_mutex);
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
#endif
		}
	}
}

void Terrain::CalculateNormals(const std::vector<unsigned int>& indices, std::vector<VertexInfo>& inOutVertices) const
{
	const unsigned int indicesSize = static_cast<unsigned int>(indices.size() / 3);
	const unsigned int verticesSize = static_cast<unsigned int>(inOutVertices.size());

	for (unsigned int t = 0; t < indicesSize; ++t)
	{
		int triangleIndex = t * 3;
		int indA = indices[triangleIndex + 0];
		int indB = indices[triangleIndex + 1];
		int indC = indices[triangleIndex + 2];

		glm::vec3 normal = CalculateNormalFromIndices(inOutVertices, indA, indB, indC);
		inOutVertices[indA].Normal += normal;
		inOutVertices[indB].Normal += normal;
		inOutVertices[indC].Normal += normal;
	}

	for (unsigned int v = 0; v < verticesSize; ++v)
	{
		inOutVertices[v].Normal = glm::normalize(inOutVertices[v].Normal);
	}
}

void Terrain::UpdateHeightMap()
{
	int length = static_cast<int>(m_planeSize.x);
	int width = static_cast<int>(m_planeSize.y);

	auto& vertices = m_mesh.GetVertices();

	for (int z = 0; z < length; ++z)
	{
		for (int x = 0; x < width; ++x)
		{
			int index = (z * width + x) * 4;

			vertices[index + 0].Position.y = GetPerlinNoise(glm::vec2(x, z))			* m_heightSize;
			vertices[index + 1].Position.y = GetPerlinNoise(glm::vec2(x + 1, z))		* m_heightSize;
			vertices[index + 2].Position.y = GetPerlinNoise(glm::vec2(x, z + 1))		* m_heightSize;
			vertices[index + 3].Position.y = GetPerlinNoise(glm::vec2(x + 1, z + 1))	* m_heightSize;
		}
	}

	CalculateNormals(m_mesh.GetIndices(), vertices);
	m_mesh.SetVertices(vertices);
	m_mesh.CreateBuffers();
}

void Terrain::UpdateHeightMapFromImage(float* heightData)
{
	int length = static_cast<int>(m_planeSize.x);
	int width = static_cast<int>(m_planeSize.y);

	auto& vertices = m_mesh.GetVertices();

	for (int z = 0; z < length; ++z)
	{
		for (int x = 0; x < width; ++x)
		{
			int index = (z * width + x) * 4;

			int imageIndex = (z * width + x);

			vertices[index + 0].Position.y = heightData[imageIndex] * m_heightSize;
			vertices[index + 1].Position.y = heightData[imageIndex] * m_heightSize;
			vertices[index + 2].Position.y = heightData[imageIndex] * m_heightSize;
			vertices[index + 3].Position.y = heightData[imageIndex] * m_heightSize;
		}
	}

	CalculateNormals(m_mesh.GetIndices(), vertices);
	m_mesh.SetVertices(vertices);
	m_mesh.CreateBuffers();
}

glm::vec3 Terrain::CalculateNormalFromIndices(const std::vector<VertexInfo>& vertices, int a, int b, int c) const
{
	glm::vec3 pA = vertices[a].Position;
	glm::vec3 pB = vertices[b].Position;
	glm::vec3 pC = vertices[c].Position;

	glm::vec3 ab = pB - pA;
	glm::vec3 ac = pC - pA;

	return glm::normalize(glm::cross(ab, ac));
}

float Terrain::GetPerlinNoise(const glm::vec2& pos) const
{
	float total = 0.0f;
	float frequency = 1.0f;
	float amplitude = 1.0f;
	float maxVal = 0.0f;

	for (int i = 0; i < m_heightmapParams.octaves; ++i)
	{
		total += glm::perlin(pos * frequency) * amplitude;
		maxVal += amplitude;
		amplitude *= m_heightmapParams.persistence;
		frequency *= m_heightmapParams.lacunarity;
	}

	if (maxVal > 0.0f)
	{
		return total / maxVal;
	}

	return 1.0f;
}
