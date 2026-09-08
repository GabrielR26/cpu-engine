#pragma once

class Drop;
class Catcher;

class DropFactory
{
protected:
	cpu_mesh m_meshDrop;
	float m_dropHeight = 4.0f;
	float m_dropRadius = 0.25f;
	float m_dropSpeed = 1.5f;

	float m_railRadius;
	Catcher* m_catcher;

public:
	DropFactory(Catcher* _catcher, const float& _railRadius);
	virtual ~DropFactory();

	Drop* SpawnDrop();

protected:
	XMFLOAT3 RandDropPosition();
};

