#include "pch.h"
#include "Drop.h"
#include "DropManager.h"
#include "Catcher.h"

Drop::Drop(Catcher* _catcher, const float& _dropHeight, const float& _dropSpeed, const float& _dropRadius, const float& _railRadius)
{
	m_radius = _dropRadius;
	m_speed = _dropSpeed;
	m_timer = 0.f;
	m_height = _dropHeight;
	m_railRadius = _railRadius;
	m_angle = 0.f;

	assert(_catcher != nullptr);
	m_catcher = _catcher;
}

float Drop::Init(const float& _lastDropRad)
{
	XMFLOAT4 _res = RandDropPosition(_lastDropRad);
	m_angle = _res.w;
	GetTransform()->pos = XMFLOAT3(_res.x, _res.y, _res.z);

	m_emitterTail = cpuEngine.CreateParticleEmitter();
	m_emitterTail->pos = cpu::Add3(GetTransform()->pos, XMFLOAT3(0.f, m_radius, 0.f));
	m_emitterTail->rate = 0.1f;
	m_emitterTail->spread = 0.25f;
	m_emitterTail->durationMax = 0.25f;
	m_emitterTail->colorMin = cpu::ToColor(255, 0, 0);
	m_emitterTail->colorMax = cpu::ToColor(255, 125, 0);

	return m_angle;
}

void Drop::Update(const float& _dt)
{
	m_timer += _dt * 5.f;

	// "Gravity"
	GetTransform()->pos.y -= m_speed * _dt;
	XMFLOAT3 _tailPos = cpu::Add3(GetTransform()->pos, cpu::Mul3(GetTransform()->up, m_radius));
	m_emitterTail->pos = _tailPos;
	// Rotation
	//GetTransform()->SetYPR(0.f, 0.f, cpu::Lerp(-0.5f, 0.5f, sin(m_dropTimer)));
	// Touch ground
	if (GetTransform()->pos.y - m_radius <= 0)
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
	if (_dist <= m_radius + m_catcher->GetRadius())
	{
		if (GetTransform()->pos.y - m_radius < m_catcher->GetTransform()->pos.y + (m_catcher->GetHeight() * 0.5))
			cpuApp.DropExplosion(GetTransform()->pos, false);
		else
			cpuApp.DropExplosion(cpu::Add3(GetTransform()->pos, XMFLOAT3(0.f, -m_radius, 0.f)), true);
		Destroy();
	}
}

XMFLOAT4 Drop::RandDropPosition(const float& _lastDropRad)
{
	int _randDeg = rand() % (135 - 30);
	int _randSign = rand() % 2 == 0 ? -1 : 1;
	float _rad = _lastDropRad + XMConvertToRadians(((_randDeg + 45) * _randSign));
	XMFLOAT3 _dropPosition(XMScalarSin(_rad) * m_railRadius, m_height, XMScalarCos(_rad) * m_railRadius);
	return XMFLOAT4(_dropPosition.x, _dropPosition.y, _dropPosition.z, _rad);
}



void Drop::Destroy()
{
	m_emitterTail->rate = 0.0f;
	cpuEngine.Release(m_emitterTail);
	m_emitterTail = nullptr;
	cpuEngine.Release(m_entity);
	m_entity = nullptr;
	cpuApp.GetDropManager()->Release(this);
}