#include "pch.h"
#include "Drop.h"
#include "DropFactory.h"
#include "DropManager.h"
#include "Catcher.h"

DropFactory::DropFactory(Catcher* _catcher, const float& _railRadius)
{
    m_meshDrop.CreateSphere(m_dropRadius, 5, 12, CPU_ORANGE, CPU_RED);
    m_railRadius = _railRadius;
    m_catcher = _catcher;
}

DropFactory::~DropFactory()
{
}

Drop* DropFactory::SpawnDrop()
{
    Drop* _newDrop = new Drop(m_catcher);
    _newDrop->SetMesh(&m_meshDrop);
    XMFLOAT3 _dropPosition = RandDropPosition();
    _newDrop->SetPosition(_dropPosition);

    return _newDrop;
}

XMFLOAT3 DropFactory::RandDropPosition()
{
    float _catcherRad = m_catcher->GetRad();
    int _randDeg = rand() % (150 - 45);
    int _randSign = rand() % 2 == 0 ? -1 : 1;
    float _rad = _catcherRad + XMConvertToRadians(((_randDeg + 45) * _randSign));
    XMFLOAT3 _dropPosition(XMScalarSin(_rad) * m_railRadius, m_dropHeight, XMScalarCos(_rad) * m_railRadius);
    return _dropPosition;
}
