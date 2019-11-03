#include "Profiler.h"

Profiler Profiler::m_instance = Profiler();

//void Profiler::Init(Game* game)
//{
//	m_gameRef = game;
//}

double Profiler::GetTimeMS() const
{
	return 0.0;
//	return m_gameRef->GetTimeMS();
}

void Profiler::AddMoment(const ProfileData & data)
{
	m_profileData.push_back(data);
}

ProfileMoment::ProfileMoment(ProfileData * dataHandle, const char * name)
	: m_handle(dataHandle)
{
	double startTime = Profiler::Instance().GetTimeMS();
	m_handle->startMS = startTime;
	m_handle->name = std::string(name);
}

ProfileMoment::~ProfileMoment()
{
	SendData();
	delete m_handle;
}

void ProfileMoment::SendData()
{
	double endTime = Profiler::Instance().GetTimeMS();
	m_handle->durationMS = endTime - m_handle->startMS;
	Profiler::Instance().AddMoment(*m_handle);
}

