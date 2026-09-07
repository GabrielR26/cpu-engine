#pragma once

class App
{
public:
	App();
	virtual ~App();

	static App& GetInstance() { return *s_pApp; }

	void OnStart();
	void RandDropPosition();
	void OnUpdate();
	void DropCatch();
	void DropExplosion();
	void UpdateCatcherPos();
	void OnExit();
	void OnRender(int pass);

	static void MyPixelShader(cpu_ps_io& io);

private:
	inline static App* s_pApp = nullptr;

	cpu_mesh m_meshCircle1;
	cpu_mesh m_meshCircle2;
	cpu_mesh m_meshRail;
	cpu_mesh m_meshCylinder;
	cpu_mesh m_meshSphere;

	cpu_entity* m_drop;
	float m_dropHeight = 4.0f;
	float m_dropRadius = 0.25f;
	float m_dropSpeed = 1.5f;
	cpu_particle_emitter* m_dropExplosion;
	cpu_particle_emitter* m_dropCatch;

	cpu_entity* m_circle1;
	cpu_entity* m_circle2;
	cpu_entity* m_rail;
	float m_railRadius = 4.f;

	cpu_entity* m_catcher;
	float m_catcherSpringArm = 3.0f;
	float m_catcherRailPos = 0.0f;
	float m_catcherSpeed = 2.0f;
	float m_catcherHeight = 0.5f;
	float m_catcherRadius = 0.5f;
};
