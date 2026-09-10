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
	XMFLOAT3 _center(0.f, m_height, 0.f);
	XMFLOAT3 _dir = cpu::Sub3(_center, GetTransform()->pos);
	GetTransform()->dir = Normalize(_dir);
	XMFLOAT3 _right = cpu::Cross3(_dir, CPU_VEC3_UP);
	GetTransform()->right = Normalize(_right);
	GetTransform()->SetRotationFromAxes();

	m_entity->transform;
	//GetTransform()->AddYPR(0.f, 0.f, -1.f);

	m_emitterTail = cpuEngine.CreateParticleEmitter();
	m_emitterTail->pos = cpu::Add3(GetTransform()->pos, XMFLOAT3(0.f, m_radius, 0.f));
	m_emitterTail->rate = 0.1f;
	m_emitterTail->spread = 0.25f;
	m_emitterTail->durationMax = 0.25f;
	m_emitterTail->colorMin = cpu::ToColor(255, 0, 0);
	m_emitterTail->colorMax = cpu::ToColor(255, 125, 0);

	return m_angle;
}

XMFLOAT3 Drop::Normalize(DirectX::XMFLOAT3& _axe)
{
	float _axeLength = sqrt(_axe.x * _axe.x + _axe.y * _axe.y + _axe.z * _axe.z);
	XMFLOAT3 _norm(_axe.x / _axeLength, _axe.y / _axeLength, _axe.z / _axeLength);
	return _norm;
}

void Drop::Update(const float& _dt)
{
	m_timer += _dt * 5.f;
	float _sin = (sin(m_timer) * 0.5f) + 0.5f;

	// "Gravity" + Straf
	float _radLerp = cpu::Lerp(m_angle - 0.05f, m_angle + 0.05f, _sin);
	XMFLOAT2 _trigo = cpuApp.GetPositionFromTrigo(m_angle, m_railRadius);
	float _y = GetTransform()->pos.y;
	GetTransform()->SetPosition(_trigo.x, _y - (m_speed * _dt), _trigo.y);
	// Tail
	XMFLOAT3 _tailPos = cpu::Add3(GetTransform()->pos, cpu::Mul3(GetTransform()->dir, m_radius));
	m_emitterTail->pos = _tailPos;
	m_emitterTail->dir = GetTransform()->up;
	// Rotation
	//GetTransform()->AddYPR(0.f, 0.f, sin(m_timer) * 0.1f);
	//GetTransform()->rot;
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
	int _randDeg = rand() % (180 - 45);
	int _randSign = rand() % 2 == 0 ? -1 : 1;
	float _rad = _lastDropRad + XMConvertToRadians(((_randDeg + 45) * _randSign));
	XMFLOAT2 _trigo = cpuApp.GetPositionFromTrigo(_rad, m_railRadius);
	return XMFLOAT4(_trigo.x, m_height, _trigo.y, _rad);
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