#include "pch.h"
#include "Catcher.h"

Catcher::Catcher(const float& _railRadius) :
	Actor()
{
	m_entity->transform.pos.y = m_height;
	m_railRadius = _railRadius;
}

void Catcher::UpdateRailPose(const float& _dt)
{
	m_rad += m_speed * _dt;
	UpdatePosition();
}

void Catcher::Init()
{
	UpdatePosition();
}

void Catcher::UpdatePosition()
{
	XMFLOAT2 _trigoPos = cpuApp.GetPositionFromTrigo(m_rad, m_railRadius);
	m_entity->transform.SetPosition(_trigoPos.x, m_height, _trigoPos.y);
}

void Catcher::Update(const float& _dt)
{
	if (cpuInput.IsLeft())
	{
//cpuApp.UpdateCameraPosition();
		UpdateRailPose(_dt);
		cpuApp.UpdateCameraPosition();
	}
	else if (cpuInput.IsRight())
	{
		UpdateRailPose(-_dt);
		cpuApp.UpdateCameraPosition();
	}
}
