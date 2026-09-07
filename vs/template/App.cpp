#include "pch.h"

App::App()
{
	s_pApp = this;
	CPU_CALLBACK_START(OnStart);
	CPU_CALLBACK_UPDATE(OnUpdate);
	CPU_CALLBACK_EXIT(OnExit);
	CPU_CALLBACK_RENDER(OnRender);
}

App::~App()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void App::OnStart()
{
	// YOUR CODE HERE

	srand(timeGetTime());

	// Resources
	m_meshCircle1.CreateCircle(m_railRadius - 0.5f, 36, CPU_GRAY);
	m_meshCircle2.CreateCircle(m_railRadius + 1.0f, 36, CPU_GRAY);
	m_meshRail.CreateCircle(m_railRadius + 0.5f, 36, CPU_WHITE);
	for (size_t i = 0; i < 36 * 3; i += 3)
	{
		XMFLOAT3 _color(i % 2, i % 2, i % 2);
		m_meshRail.vertices[i].color = _color;
		m_meshRail.vertices[i + 1].color = _color;
		m_meshRail.vertices[i + 2].color = _color;
	}

	m_meshCylinder.CreateCylinder(m_catcherHeight, m_catcherRadius, 12, true, true, CPU_BLUE);
	m_meshSphere.CreateSphere(m_dropRadius, 5, 12, CPU_ORANGE, CPU_RED);

	// Entity
	m_circle1 = cpuEngine.CreateEntity();
	m_circle1->pMesh = &m_meshCircle1;
	m_circle1->transform.pos.y = 0.1f;
	m_circle2 = cpuEngine.CreateEntity();
	m_circle2->pMesh = &m_meshCircle2;
	m_circle2->transform.pos.y = -0.1f;
	m_rail = cpuEngine.CreateEntity();
	m_rail->pMesh = &m_meshRail;
	m_catcher = cpuEngine.CreateEntity();
	m_catcher->pMesh = &m_meshCylinder;
	m_catcher->transform.pos.y = m_catcherHeight;
	UpdateCatcherPos();
	m_drop = cpuEngine.CreateEntity();
	m_drop->pMesh = &m_meshSphere;
	RandDropPosition();

	// Emitter
	cpuEngine.GetParticleData()->Create(10000);
	cpuEngine.GetParticlePhysics()->gy = 0.0f;
	m_dropExplosion = cpuEngine.CreateParticleEmitter();
	m_dropExplosion->rate = 0.0f;
	m_dropExplosion->colorMin = cpu::ToColor(255, 0, 0);
	m_dropExplosion->colorMax = cpu::ToColor(255, 255, 0);
	m_dropExplosion->spread = 2.0f;
	m_dropExplosion->durationMax = 1.0f;
	m_dropCatch = cpuEngine.CreateParticleEmitter();
	m_dropCatch->rate = 0.0f;
	m_dropCatch->colorMin = cpu::ToColor(0, 255, 0);
	m_dropCatch->colorMax = cpu::ToColor(0, 255, 255);
	m_dropCatch->spread = 2.0f;
	m_dropCatch->durationMax = 1.0f;
}

void App::RandDropPosition()
{
	int _rand = rand() % 360;
	float _rad = XMConvertToRadians(_rand);
	XMFLOAT3 _dropPosition(XMScalarSin(_rad) * m_railRadius, m_dropHeight, XMScalarCos(_rad) * m_railRadius);
	m_drop->transform.pos = _dropPosition;
}

void App::OnUpdate()
{
	// YOUR CODE HERE

	float _dt = cpuTime.delta;
	float _time = cpuTime.total;

	// Disable rate for small explosion
	if (m_dropExplosion->rate > 0.0f)
		m_dropExplosion->rate = 0.0f;
	else if (m_dropCatch->rate > 0.0f)
		m_dropCatch->rate = 0.0f;

	// Input
	if (cpuInput.IsLeft())
	{
		m_catcherRailPos += m_catcherSpeed * _dt;
		UpdateCatcherPos();
	}
	else if (cpuInput.IsRight())
	{
		m_catcherRailPos -= m_catcherSpeed * _dt;
		UpdateCatcherPos();
	}
	float _dropSpeed = m_dropSpeed;
	if (cpuInput.IsDown())
		_dropSpeed *= 2;

	// Drops
	m_drop->transform.pos.y -= _dropSpeed * _dt;

	if (m_drop->transform.pos.y - m_dropRadius <= 0)
		DropExplosion();

	float _diffX = m_drop->transform.pos.x - m_catcher->transform.pos.x;
	float _diffY = m_drop->transform.pos.y - m_catcher->transform.pos.y;
	float _diffZ = m_drop->transform.pos.z - m_catcher->transform.pos.z;
	float _dist = sqrt(_diffX * _diffX + _diffY * _diffY + _diffZ * _diffZ);
	if (_dist <= m_dropRadius + m_catcherRadius)
	{
		if (m_drop->transform.pos.y - m_dropRadius < m_catcher->transform.pos.y + (m_catcherHeight * 0.5))
			DropExplosion();
		else
			DropCatch();
	}

	// Quit
	if (cpuInput.IsBackPressed())
		cpuEngine.Quit();
}

void App::DropCatch()
{
	m_dropCatch->pos = m_drop->transform.pos;
	m_dropCatch->rate = 1.0f;
	RandDropPosition();
}

void App::DropExplosion()
{
	m_dropExplosion->pos = m_drop->transform.pos;
	m_dropExplosion->rate = 1.0f;
	RandDropPosition();
}

void App::UpdateCatcherPos()
{
	float _sin = XMScalarSin(m_catcherRailPos);
	float _cos = XMScalarCos(m_catcherRailPos);

	XMFLOAT3 _catcherPos = m_catcher->transform.pos;
	_catcherPos.x = _sin * m_railRadius;
	_catcherPos.z = _cos * m_railRadius;
	m_catcher->transform.pos = _catcherPos;

	cpuEngine.GetCamera()->transform.SetPosition(_catcherPos.x * m_catcherSpringArm, m_catcherSpringArm, _catcherPos.z * m_catcherSpringArm);
	cpuEngine.GetCamera()->transform.LookAt(0, 0, 0);
}

void App::OnExit()
{
	// YOUR CODE HERE
}

void App::OnRender(int pass)
{
	// YOUR CODE HERE
}

void App::MyPixelShader(cpu_ps_io& io)
{
	// YOUR CODE HERE
	io.color = io.p.color;
}
