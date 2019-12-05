
set(MODULE_NAME Core)

add_library(
	${MODULE_NAME} STATIC 

	Assets/AssetId.cpp
	Assets/AssetId.h
	Assets/AssetManager.cpp
	Assets/AssetManager.h
	Assets/AssimpImporter.cpp
	Assets/AssimpImporter.h
	Assets/ShaderManager.cpp
	Assets/ShaderManager.h
	Assets/TextureManager.cpp
	Assets/TextureManager.h
	Assets/ThreadSafeQueue.h

	Components/System/FpsCounterSystemComponent.cpp
	Components/System/FpsCounterSystemComponent.h
	Components/System/SceneCameraComponent.cpp
	Components/System/SceneCameraComponent.h

	External/Imgui/imgui_impl_opengl3.cpp
	External/Imgui/imgui_impl_sdl.cpp
	External/Imgui/imgui_stdlib.cpp
	External/Imgui/imgui_stdlib.h

	Renderer/RenderPass.h
	Renderer/Renderer.cpp
	Renderer/Renderer.h
	Renderer/Shader.cpp
	Renderer/Shader.h
	Renderer/Material.cpp
	Renderer/Material.h
	Renderer/Light.cpp
	Renderer/Light.h
	
	Systems/Camera/Camera.cpp
	Systems/Camera/Camera.h
	Systems/Camera/FlyCamera.cpp
	Systems/Camera/FlyCamera.h

	Systems/Geometry/BoundingBox.cpp
	Systems/Geometry/BoundingBox.h
	Systems/Geometry/BoundingFrustum.cpp
	Systems/Geometry/BoundingFrustum.h
	Systems/Geometry/BoundingSphere.cpp
	Systems/Geometry/BoundingSphere.h
	Systems/Geometry/GeomDefines.h
	Systems/Geometry/Plane.cpp
	Systems/Geometry/Plane.h
	Systems/Geometry/Rect.cpp
	Systems/Geometry/Rect.h

	Systems/PostProcessing/SSAO.cpp
	Systems/PostProcessing/SSAO.h

	Systems/Rendering/Primitives/Quad.cpp
	Systems/Rendering/Primitives/Quad.h
	Systems/Rendering/Primitives/PlanePrimitive.cpp
	Systems/Rendering/Primitives/PlanePrimitive.h
	Systems/Rendering/Primitives/Sphere.cpp
	Systems/Rendering/Primitives/Sphere.h
	Systems/Rendering/Primitives/Cube.cpp
	Systems/Rendering/Primitives/Cube.h

	Systems/Rendering/Gizmo.cpp
	Systems/Rendering/Gizmo.h
	Systems/Rendering/IBLSkybox.cpp
	Systems/Rendering/IBLSkybox.h
	Systems/Rendering/Mesh.cpp
	Systems/Rendering/Mesh.h
	Systems/Rendering/Primitives.cpp
	Systems/Rendering/Primitives.h

	Systems/Rendering/Skybox.cpp
	Systems/Rendering/Skybox.h
	Systems/Rendering/Terrain.cpp
	Systems/Rendering/Terrain.h
	Systems/Rendering/TextureBuffer.cpp
	Systems/Rendering/TextureBuffer.h
	Systems/Rendering/Texture.h
	Systems/Rendering/Vertex.h

	Systems/UI/UIHelper.h

	Systems/Entity.cpp
	Systems/Entity.h
	Systems/FileIO.h
	Systems/Model.cpp
	Systems/Model.h
	Systems/Octree.cpp
	Systems/Octree.h
	Systems/QuadTree.cpp
	Systems/QuadTree.h
	Systems/Transform.cpp
	Systems/Transform.h

	Game.cpp
	Game.h
	IMGUIHandler.cpp
	IMGUIHandler.h
	Profiler.cpp
	Profiler.h
	SDLHandler.cpp
	SDLHandler.h
	State.h
	SystemComponentManager.cpp
	SystemComponentManager.h
	Utils.cpp
	Utils.h
	WindowParams.cpp
	WindowParams.h
	
)

# Find and target external libs
find_package(glm CONFIG REQUIRED)
target_link_libraries(${MODULE_NAME} glm)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(${MODULE_NAME} imgui::imgui)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${MODULE_NAME} nlohmann_json nlohmann_json::nlohmann_json)

find_package(GLEW REQUIRED)
target_link_libraries(${MODULE_NAME} GLEW::GLEW)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(${MODULE_NAME} assimp::assimp)

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(${MODULE_NAME} SDL2::SDL2 SDL2::SDL2main)

# target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})