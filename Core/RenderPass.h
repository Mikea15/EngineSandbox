
#include "Systems/Light.h"
#include "Systems/Entity.h"
#include "Systems/Material.h"
#include "Systems/Camera/Camera.h"

class RenderPass
{
public:

};


class ShadowRenderPass
{
public:
	void GatherLights(std::vector<std::shared_ptr<ILight>>& lights) 
	{

	}

	void GatherObjects(std::vector<std::shared_ptr<Entity>>& entities) 
	{
		
	}

	void Render(Camera& camera,
		const std::vector<std::shared_ptr<ILight>>& lights, 
		const std::vector<std::shared_ptr<Entity>>& entities,
		Material& mat) 
	{
		for (auto& entity : entities)
		{
			for (auto& light : lights)
			{
				entity->ApplyLightingPass(*light);
			}
			glm::mat4 model = entity->GetTransform().GetModelMat();
			entity->Draw(model, camera.GetView(), camera.GetProjection(), mat);
		}
	}
};
