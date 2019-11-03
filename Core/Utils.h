#pragma once

namespace Utils
{
	static float Lerp(float a, float b, float t)
	{
		return a + t * (b - a);
	}
}
