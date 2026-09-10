#pragma once
#include "Actor.h"

class Catcher;

class Drop :
	public Actor
{
protected:
	float m_radius;
	float m_speed;
	float m_timer;
	float m_height;
	float m_angle;
	float m_railRadius;
	cpu_particle_emitter* m_emitterTail;
	Catcher* m_catcher;

public:
	Drop(Catcher* _catcher, const float& _dropHeight, const float& _dropSpeed, const float& _dropRadius, const float& _railRadius);
	virtual ~Drop() = default;

	float Init(const float& _lastDropRad);
	XMFLOAT3 Normalize(DirectX::XMFLOAT3& _axe);
	void Destroy();

	// Hérité via Actor
	void Update(const float& _dt) override;

protected:
	XMFLOAT4 RandDropPosition(const float& _lastDropRad);
};

