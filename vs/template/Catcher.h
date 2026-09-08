#pragma once
#include "Actor.h"

class Catcher :
    public Actor
{
protected:
    float m_height = 0.5f;
    float m_radius = 0.5f;
    float m_springArm = 3.0f;
    float m_rad = 0.0f;
    float m_speed = 2.0f;
    float m_railRadius;

public:
    Catcher(const float& _railRadius);
    virtual ~Catcher() = default;
    void Init();

    float GetHeight() { return m_height; }
    float GetRadius() { return m_radius; }
    float GetSpringArm() { return m_springArm; }
    float GetRad() { return m_rad; }

    void UpdateRailPose(const float& _dt);

    // Hérité via Actor
    void Update(const float& _dt) override;

protected:
    void UpdatePosition();
};

