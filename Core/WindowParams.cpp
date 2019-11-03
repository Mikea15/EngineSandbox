
#include "WindowParams.h"

void to_json(json& j, const WindowParams& p)
{
	j = json{
		{ "WindowName", p.WindowName },
		{ "Resolution", {
			{"Height", p.Height},
			{"Width", p.Width},
			{"Depth", p.Depth},
			{"ResolutionIndex", p.ResolutionIndex},
			{"DisplayIndex", p.DisplayIndex}
		}
		},
		{"Fullscreen", p.Fullscreen},
		{"VSync", p.VSync},
		{"FPSLimit", p.FPSLimit},
		{"GL_MultiSampleBuffers", p.GL_MultiSampleBuffers },
		{"GL_MultiSamplesSamples", p.GL_MultiSamplesSamples }
	};
}

void from_json(const json& j, WindowParams& p)
{
	j.at("WindowName").get_to(p.WindowName);

	auto resolution = j.at("Resolution");
	resolution.at("Height").get_to(p.Height);
	resolution.at("Width").get_to(p.Width);
	resolution.at("Depth").get_to(p.Depth);
	resolution.at("ResolutionIndex").get_to(p.ResolutionIndex);
	resolution.at("DisplayIndex").get_to(p.DisplayIndex);

	j.at("Fullscreen").get_to(p.Fullscreen);
	j.at("VSync").get_to(p.VSync);
	j.at("FPSLimit").get_to(p.FPSLimit);
	j.at("GL_MultiSampleBuffers").get_to(p.GL_MultiSampleBuffers);
	j.at("GL_MultiSamplesSamples").get_to(p.GL_MultiSamplesSamples);
}
