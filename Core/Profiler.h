#pragma once

#include <string>
#include <vector>

struct ProfileData
{
	double startMS;
	double durationMS;
	std::string name;
};

class Profiler
{
public:
	static Profiler& Instance() { return m_instance; }
	~Profiler() = default;

	//void Init(Game* game);
	
	double GetTimeMS() const;

	void AddMoment(const ProfileData& data);

	std::vector<ProfileData>* GetProfileData() { return &m_profileData; }

private:
	//Game* m_gameRef;

	static Profiler m_instance;
	std::vector<ProfileData> m_profileData;
};

#define PROFILE(name)						\
 ProfileMoment( new ProfileData(), name );	\


class ProfileMoment
{
public:
	ProfileMoment(ProfileData* dataHandle, const char* name);
	~ProfileMoment();

	void SendData();

private:
	ProfileData* m_handle;
};
