#pragma once
#include "Actor.h"
class Decor :
    public Actor
{
public:
    Decor() = default;
    virtual ~Decor() = default;

    // Hérité via Actor
    void Update(const float& _dt) override;
};

