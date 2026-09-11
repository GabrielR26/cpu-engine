#include "pch.h"
#include "Drop.h"
#include "DropFactory.h"
#include "DropManager.h"
#include "Catcher.h"

DropFactory::DropFactory(Catcher* _catcher, const float& _railRadius)
{
    m_dropHeight = 4.25f;
    m_dropRadius = 0.25f;
    m_dropSpeed = 1.5f;
    m_lastDropRad = 0.f;

    m_meshDrop.CreateSphere(m_dropRadius, 5, 12, CPU_ORANGE, CPU_RED);
    m_railRadius = _railRadius;
    m_catcher = _catcher;
}

Drop* DropFactory::SpawnDrop()
{
    Drop* _newDrop = new Drop(m_catcher, m_dropHeight, m_dropSpeed, m_dropRadius, m_railRadius);
    m_lastDropRad = _newDrop->Init(m_lastDropRad);
    _newDrop->SetMesh(&m_meshDrop);

    return _newDrop;
}