#pragma once

#include "AssetId.h"

class BaseResource
{
public:
	BaseResource() {}
	virtual ~BaseResource() {}

	AssetId GetId() const { return m_id; }
	bool IsValid() const { return m_id.IsValid(); }

private:
	AssetId m_id;
};