#pragma once

#include "AssetId.h"

class BaseResource
{
public:
	BaseResource() {}
	virtual ~BaseResource() {}

	AssetId GetId() const { return m_id; }
	virtual bool IsValid() const { return m_id.IsValid(); }

protected:
	AssetId m_id;
};