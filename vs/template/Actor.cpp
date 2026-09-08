#include "pch.h"
#include "Actor.h"
//#include "../cpu-engine/cpu_entity.h"

Actor::Actor()
{
	m_entity = cpuEngine.CreateEntity();
}

Actor::~Actor()
{
	//delete m_actor;
}

void Actor::SetMesh(cpu_mesh* _mesh)
{
	m_entity->pMesh = _mesh;
}
