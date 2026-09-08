#include "pch.h"
#include "Drop.h"
#include "DropManager.h"
#include "Catcher.h"

Drop::Drop(Catcher* _catcher)
{
	assert(_catcher != nullptr);
	m_catcher = _catcher;
}

void Drop::Update(const float& _dt)
{
	// "Gravity"
	GetTransform()->pos.y -= m_dropSpeed * _dt;
	// Touch ground
	if (GetTransform()->pos.y - m_dropRadius <= 0)
	{
		cpuApp.DropExplosion(GetTransform()->pos, false);
		Destroy();
		return;
	}
	// Touch Catcher
	float _diffX = GetTransform()->pos.x - m_catcher->GetTransform()->pos.x;
	float _diffY = GetTransform()->pos.y - m_catcher->GetTransform()->pos.y;
	float _diffZ = GetTransform()->pos.z - m_catcher->GetTransform()->pos.z;
	float _dist = sqrt(_diffX * _diffX + _diffY * _diffY + _diffZ * _diffZ);
	if (_dist <= m_dropRadius + m_catcher->GetRadius())
	{
		if (GetTransform()->pos.y - m_dropRadius < m_catcher->GetTransform()->pos.y + (m_catcher->GetHeight() * 0.5))
			cpuApp.DropExplosion(GetTransform()->pos, false);
		else
			cpuApp.DropExplosion(GetTransform()->pos, true);
		Destroy();
	}
}

void Drop::Destroy()
{
	cpuEngine.Release(m_entity);
	m_entity = nullptr;
	cpuApp.GetDropManager()->Release(this);
}