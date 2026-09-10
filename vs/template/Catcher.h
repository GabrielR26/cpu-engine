#pragma once
#include "Actor.h"

class Catcher :
	public Actor
{
protected:
	float m_height;
	float m_radius;
	float m_springArm;
	float m_angle;
	float m_angleLag;
	float m_maxAngleLag;
	float m_maxLag;
	float m_speed;
	float m_railRadius;

	float m_lagTimerStop;
	float m_lagTimerMove;
	float m_lagTimeStop;
	float m_lagTimeMove;
	float m_signMemory;

	cpu_entity* m_ghost;
	cpu_mesh m_meshGhost;

public:
	Catcher(const float& _railRadius);
	virtual ~Catcher() = default;
	void Init();

	const float GetHeight() { return m_height; }
	const float GetRadius() { return m_radius; }
	const float GetSpringArm() { return m_springArm; }
	const float GetAngle() { return m_angle; }
	const float GetAngleLag() { return m_angleLag; }

	void UpdateRailPose(const float& _dt, const float& _sign);
	void UpdatePosition();

	// Hérité via Actor
	void Update(const float& _dt) override;
};

