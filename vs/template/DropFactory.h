#pragma once

class Drop;
class Catcher;

class DropFactory
{
protected:
	cpu_mesh m_meshDrop;
	float m_dropHeight;
	float m_dropRadius;
	float m_dropSpeed;
	float m_lastDropRad;

	float m_railRadius;
	Catcher* m_catcher;

public:
	DropFactory(Catcher* _catcher, const float& _railRadius);

	Drop* SpawnDrop();
};

