#include "pch.h"
#include "Catcher.h"

Catcher::Catcher(const float& _railRadius) :
	Actor()
{
	m_height = 0.5f;
	m_radius = 0.75f;
	m_springArm = 3.0f;
	m_angle = 0.0f;
	m_angleLag = 0.0f;
	m_maxLag = 0.15f;
	m_speed = 1.75f;

	m_lagTimeStop = 0.5f;
	m_lagTimeMove = 1.f;
	m_lagTimerStop = 0.f;
	m_lagTimerMove = 0.f;
	m_signMemory = 0.f;

	m_entity->transform.pos.y = m_height;
	m_railRadius = _railRadius;

	m_ghost = cpuEngine.CreateEntity();
	m_meshGhost.CreateCylinder(m_height, m_radius * 0.25f, 12, true, true, CPU_GRAY);
	m_ghost->pMesh = &m_meshGhost;
}

void Catcher::Init()
{
	UpdatePosition();
}

void Catcher::UpdateRailPose(const float& _dt, const float& _sign)
{
	if (m_signMemory != _sign)
	{
		m_lagTimerMove = 0.f;
		m_signMemory = _sign;
	}
	m_lagTimerStop = 0.f;
	m_lagTimerMove += _dt;

	m_angle += m_speed * (_dt * _sign);
	m_maxAngleLag = m_angle - (m_maxLag * _sign);
	m_angleLag = cpu::Lerp(m_angleLag, m_maxAngleLag, cpu::Clamp(m_lagTimerMove / m_lagTimeMove));

	UpdatePosition();
}

void Catcher::UpdatePosition()
{
	XMFLOAT2 _trigoPos = cpuApp.GetPositionFromTrigo(m_angle, m_railRadius);
	m_entity->transform.SetPosition(_trigoPos.x, m_height, _trigoPos.y);
	m_ghost->transform.pos = cpu::Mul3(XMFLOAT3(_trigoPos.x, -m_height, _trigoPos.y), -1.f);
}

void Catcher::Update(const float& _dt)
{
	if (cpuInput.IsLeft())
	{
		UpdateRailPose(_dt, 1.f);
		cpuApp.UpdateCameraPosition();
		return;
	}
	else if (cpuInput.IsRight())
	{
		UpdateRailPose(_dt, -1.f);
		cpuApp.UpdateCameraPosition();
		return;
	}
	if (cpuInput.IsSpacePressed())
	{
		m_angle += m_angle > 0 ? -XM_PI : XM_PI;
		UpdatePosition();
		m_lagTimerStop = m_lagTimeStop;
	}

	// Catcher stoped
	m_lagTimerMove = 0.f;
	m_lagTimerStop += _dt;
	m_angleLag = cpu::Lerp(m_angleLag, m_angle, cpu::Clamp(m_lagTimerStop / m_lagTimeStop));
	cpuApp.UpdateCameraPosition();
}
