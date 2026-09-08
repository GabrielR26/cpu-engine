#pragma once

class cpu_entity;

class Actor : public cpu_object
{
protected:
	cpu_entity* m_entity;

public:
	Actor();
	virtual ~Actor();

	cpu_transform* GetTransform() { return &m_entity->transform; }
	void SetMesh(cpu_mesh* _mesh);
	void SetPosition(const XMFLOAT3& _pos) { m_entity->transform.pos = _pos; }

	virtual void Update(const float& _dt) = 0;
};

