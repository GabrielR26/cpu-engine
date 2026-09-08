#pragma once
#include "Actor.h"

class Catcher;

class Drop :
    public Actor
{
protected:
    float m_dropHeight = 4.0f;
    float m_dropRadius = 0.25f;
    float m_dropSpeed = 1.5f;
    Catcher* m_catcher;

public:
    Drop(Catcher* _catcher);
    virtual ~Drop() = default;

    void Destroy();

    // Hérité via Actor
    void Update(const float& _dt) override;
};

