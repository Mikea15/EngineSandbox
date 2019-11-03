#include "Entity.h"

unsigned int Entity::ID = 0;

Entity::Entity()
	: m_id(++ID)
{
}

Entity::Entity(const Entity& copy)
	: m_id(copy.m_id)
	, m_transform(copy.m_transform)
	, m_model(copy.m_model)
{
}

Entity::Entity(Entity&& move)
	: m_id(move.m_id)
	, m_transform(move.m_transform)
	, m_model(move.m_model)
{

}

Entity& Entity::operator=(const Entity& assign)
{
	m_id = assign.m_id;
	m_transform = assign.m_transform;
	m_model = assign.m_model;
	return *this;
}

void Entity::Update(float deltaTime)
{
	// for each component, 
	// check if component active
	// update component.
}

void Entity::Draw(const glm::mat4& view, const glm::mat4& projection)
{
	m_model.Draw(m_transform.GetModelMat(), view, projection);
}

