
#include "Rect.h"

Rect::Rect()
	: m_origin(0.0f), m_halfSize(0.0f)
{
}

Rect::Rect(const glm::vec2 & origin, float halfSize)
	: m_origin(origin), m_halfSize(halfSize)
{
	m_min = m_origin + glm::vec2(-m_halfSize, -m_halfSize);
	m_max = m_origin + glm::vec2(m_halfSize, m_halfSize);
}

Rect::~Rect()
{
}

bool Rect::Intersect(const glm::vec2& point) const
{
	if (point.x < m_min.x || point.x > m_max.x) return false;
	if (point.y < m_min.y || point.y > m_max.y) return false;
	return true;
}

bool Rect::Intersect(const Rect& rect) const 
{
	if (rect.m_max.x < m_min.x || rect.m_min.x > m_max.x) return false;
	if (rect.m_max.y < m_min.y || rect.m_min.y > m_max.y) return false;
	return true;
}
