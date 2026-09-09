#include "pch.h"
#include "Catcher.h"
#include "Decor.h"
#include "Drop.h"
#include "DropFactory.h"
#include "DropManager.h"

App::App()
{
	s_pApp = this;
	CPU_CALLBACK_START(OnStart);
	CPU_CALLBACK_UPDATE(OnUpdate);
	CPU_CALLBACK_EXIT(OnExit);
	CPU_CALLBACK_RENDER(OnRender);

	// Init var
	m_gameState = GameState::Start;
	m_life = 100;
	m_score = 0;
	m_time = 0.f;
	m_stateTimer = 0.f;
	m_startTime = 3.f;
	m_startOffset = 20.f;
	m_railRadius = 4.f;
	m_dropTimer = 0.f;
	m_dropTime = 1.5f;
	m_overTime = 0.f;

	m_emitterDropSpawn = nullptr;
	m_emitterDropExplosion = nullptr;
	m_emitterDropCatch = nullptr;
	m_catcher = nullptr;
	m_circle1 = nullptr;
	m_circle2 = nullptr;
	m_rail = nullptr;
	m_dropManager = nullptr;
	m_dropFactory = nullptr;
}

App::~App()
{
}

void App::OnStart()
{
	// YOUR CODE HERE

	srand(timeGetTime());

	// Font
	int _fontSize = cpuDevice.GetHeight() <= 512 ? 14 : 28;
	m_font.Create(_fontSize);
	m_titlefont.Create(_fontSize * 5);

	// Decor
	m_meshCircle1.CreateCircle(m_railRadius - 0.5f, 36, CPU_GRAY);
	m_meshCircle2.CreateCircle(m_railRadius + 1.0f, 36, CPU_GRAY);
	m_meshRail.CreateCircle(m_railRadius + 0.5f, 36, CPU_WHITE);
	for (int i = 0; i < 36 * 3; i += 3)
	{
		int _mod = i % 2;
		XMFLOAT3 _color((float)_mod, (float)_mod, (float)_mod);
		m_meshRail.vertices[i].color = _color;
		m_meshRail.vertices[i + 1].color = _color;
		m_meshRail.vertices[i + 2].color = _color;
	}
	m_circle1 = new Decor();
	m_circle1->SetMesh(&m_meshCircle1);
	m_circle1->GetTransform()->pos.y = 0.1f;
	m_circle2 = new Decor();
	m_circle2->SetMesh(&m_meshCircle2);
	m_circle2->GetTransform()->pos.y = -0.1f;
	m_rail = new Decor();
	m_rail->SetMesh(&m_meshRail);

	// Catcher
	m_catcher = new Catcher(m_railRadius);
	m_meshCatcher.CreateCylinder(m_catcher->GetHeight(), m_catcher->GetRadius(), 12, true, true, CPU_BLUE);
	m_catcher->SetMesh(&m_meshCatcher);
	m_catcher->Init();
	UpdateCameraPosition();

	// Drop	
	m_dropManager = new DropManager();
	m_dropFactory = new DropFactory(m_catcher, m_railRadius);

	// Emitter
	cpuEngine.GetParticleData()->Create(1000000);
	cpuEngine.GetParticlePhysics()->gy = 0.0f;
	m_emitterDropExplosion = cpuEngine.CreateParticleEmitter();
	m_emitterDropExplosion->rate = 0.1f;
	m_emitterDropExplosion->spread = 1.0f;
	m_emitterDropExplosion->durationMax = 1.0f;
	m_emitterDropExplosion->colorMin = cpu::ToColor(255, 125, 125);
	m_emitterDropExplosion->colorMax = cpu::ToColor(255, 125, 0);
	m_emitterDropSpawn = cpuEngine.CreateParticleEmitter();
	m_emitterDropSpawn->rate = 0.1f;
	m_emitterDropSpawn->spread = 1.0f;
	m_emitterDropSpawn->durationMax = 1.0f;
	m_emitterDropSpawn->colorMin = cpu::ToColor(0, 0, 255);
	m_emitterDropSpawn->colorMax = cpu::ToColor(0, 125, 255);
	m_emitterDropCatch = cpuEngine.CreateParticleEmitter();
	m_emitterDropCatch->rate = 0.1f;
	m_emitterDropCatch->spread = 1.0f;
	m_emitterDropCatch->durationMax = 1.0f;
	m_emitterDropCatch->colorMin = cpu::ToColor(0, 255, 0);
	m_emitterDropCatch->colorMax = cpu::ToColor(125, 255, 125);
}

void App::OnUpdate()
{
	// YOUR CODE HERE

	m_stateTimer += cpuTime.delta;

	switch (m_gameState)
	{
	case Start:
		StartUpdate();
		break;
	case Engage:
		EngageUpdate();
		break;
	case Game:
		GameUpdate();
		break;
	case Over:
		OverUpdate();
		break;
	}

	// Quit
	if (cpuInput.IsBackPressed())
		cpuEngine.Quit();
}

void App::OnExit()
{
	// YOUR CODE HERE

	delete m_catcher;
	m_catcher = nullptr;

	delete m_circle1;
	m_circle1 = nullptr;
	delete m_circle2;
	m_circle2 = nullptr;
	delete m_rail;
	m_rail = nullptr;

	delete m_dropFactory;
	m_dropFactory = nullptr;

	m_dropManager->Purge();
	delete m_dropManager;
	m_dropManager = nullptr;
}

void App::OnRender(int pass)
{
	// YOUR CODE HERE

	// UI
	XMFLOAT3 _tintWhite = { 1.0f, 1.0f, 1.0f };
	XMFLOAT3 _tintBlack = { 0.f, 0.f, 0.f };
	XMFLOAT3 _tintRed = { 1.0f, 0.1f, 0.1f };
	XMFLOAT3 _tintGreen = { 0.1f, 1.0f, 0.1f };
	XMFLOAT3 _tintBlue = { 0.1f, 0.1f, 1.0f };

	switch (m_gameState)
	{
	case Start:
	{
		std::string _gameOver = "READY ?";
		cpuDevice.DrawText(&m_titlefont, _gameOver.c_str(), (int)(cpuDevice.GetWidth() * 0.5f), (int)(cpuDevice.GetHeight() * 0.2f), CPU_TEXT_CENTER, &_tintWhite);
	}
	break;
	case Engage:
	{
		std::string _gameOver = "GO !!!";
		cpuDevice.DrawText(&m_titlefont, _gameOver.c_str(), (int)(cpuDevice.GetWidth() * 0.5f), (int)(cpuDevice.GetHeight() * 0.2f), CPU_TEXT_CENTER, &_tintWhite);
	}
	break;
	case Game:
	{
		std::string _life = "Life: " + CPU_STR(m_life);
		std::string _score = "Score: " + CPU_STR(m_score);
		std::string _time = "Time: " + CPU_STR((int)m_stateTimer);
		cpuDevice.DrawText(&m_font, _life.c_str(), (int)(cpuDevice.GetWidth() * 0.1f), 10, CPU_TEXT_CENTER, &_tintGreen);
		cpuDevice.DrawText(&m_font, _score.c_str(), (int)(cpuDevice.GetWidth() * 0.5f), 10, CPU_TEXT_CENTER, &_tintBlue);
		cpuDevice.DrawText(&m_font, _time.c_str(), (int)(cpuDevice.GetWidth() * 0.9f), 10, CPU_TEXT_CENTER, &_tintRed);
	}
	break;
	case Over:
	{
		std::string _gameOver = "GAME OVER";
		std::string _score = "Score: " + CPU_STR(m_score);
		std::string _time = "Time: " + CPU_STR((int)m_overTime);
		cpuDevice.DrawText(&m_font, _score.c_str(), (int)(cpuDevice.GetWidth() * 0.3f), 10, CPU_TEXT_CENTER, &_tintBlue);
		cpuDevice.DrawText(&m_font, _time.c_str(), (int)(cpuDevice.GetWidth() * 0.7f), 10, CPU_TEXT_CENTER, &_tintRed);
		cpuDevice.DrawText(&m_titlefont, _gameOver.c_str(), (int)(cpuDevice.GetWidth() * 0.5f), (int)(cpuDevice.GetHeight() * 0.75f), CPU_TEXT_CENTER, &_tintWhite);
	}
	break;
	}
}

void App::StartUpdate()
{
	float _delta = cpu::Clamp(m_stateTimer / m_startTime);
	float _catcherSpringArm = m_catcher->GetSpringArm();

	// Zoom length
	float _camRadius = cpu::Lerp(m_startOffset, m_railRadius * _catcherSpringArm, _delta);
	// Zoom height
	float _camHeight = cpu::Lerp(_catcherSpringArm * 3, _catcherSpringArm, _delta);
	// Rotation
	float _camRad = cpu::Lerp(0.f, XM_2PI, _delta);
	XMFLOAT2 _camRot2 = GetPositionFromTrigo(_camRad, _camRadius);
	/// Camera
	XMFLOAT3 _camRot3(_camRot2.x, _camHeight, _camRot2.y);
	cpuEngine.GetCamera()->transform.pos = _camRot3;
	cpuEngine.GetCamera()->transform.LookAt(0, 0, 0);

	/// Emitter
	m_emitterDropExplosion->pos = RandXYPos(10);
	m_emitterDropCatch->pos = RandXYPos(10);
	m_emitterDropSpawn->pos = RandXYPos(10);

	if (m_stateTimer >= 3.f)
	{
		m_emitterDropExplosion->rate = 0.0f;
		m_emitterDropCatch->rate = 0.0f;
		m_emitterDropSpawn->rate = 0.0f;

		m_emitterDropCatch->spread = 0.5f;

		ToState(GameState::Engage);
	}
}

void App::EngageUpdate()
{
	if (m_stateTimer >= 0.75f)
		ToState(GameState::Game);
}

void App::GameUpdate()
{
	float _dt = cpuTime.delta;
	cpu::Clamp(m_dropTime, 1.f, m_dropTime -= _dt * _dt);

	// Disable rate for small explosion
	if (m_emitterDropExplosion->rate > 0)
		m_emitterDropExplosion->rate = 0.0f;
	if (m_emitterDropCatch->rate > 0)
		m_emitterDropCatch->rate = 0.0f;
	if (m_emitterDropSpawn->rate > 0)
		m_emitterDropSpawn->rate = 0.0f;

	if (m_dropTimer >= m_dropTime)
	{
		Drop* _newDrop = m_dropFactory->SpawnDrop();
		m_dropManager->Add(_newDrop);
		m_emitterDropSpawn->pos = _newDrop->GetTransform()->pos;
		m_emitterDropSpawn->rate = 0.5f;
		m_dropTimer = 0.f;
	}
	else
		m_dropTimer += _dt;

	// Catcher
	m_catcher->Update(_dt);

	//// Drops
	for (size_t i = 0; i < m_dropManager->count; i++)
		m_dropManager->list[i]->Update(_dt);
	m_dropManager->Purge();
	//OutputDebugStringA(std::to_string(m_dropManager->count).c_str());
}

void App::OverUpdate()
{
	m_emitterDropExplosion->rate = 0.0f;
	m_emitterDropCatch->rate = 0.0f;
}

void App::ToState(GameState _state)
{
	m_gameState = _state;
	m_stateTimer = 0.f;
}

void App::UpdateCameraPosition()
{
	//cpuEngine.GetCamera()->transform.SetPosition(m_catcher->GetTransform()->pos.x * _catcherSpringArm, _catcherSpringArm, m_catcher->GetTransform()->pos.z * _catcherSpringArm);

	float _catcherSpringArm = m_catcher->GetSpringArm();
	float _catcherAngle = m_catcher->GetAngleLag();
	XMFLOAT2 _catcherTrigo = GetPositionFromTrigo(_catcherAngle, m_railRadius);
	cpuEngine.GetCamera()->transform.SetPosition(_catcherTrigo.x * _catcherSpringArm, _catcherSpringArm, _catcherTrigo.y * _catcherSpringArm);
	cpuEngine.GetCamera()->transform.LookAt(0, 0, 0);
}

void App::DropExplosion(const XMFLOAT3& _pos, const bool _isCatch)
{
	if (_isCatch)
	{
		m_emitterDropCatch->pos = _pos;
		m_emitterDropCatch->rate = 0.5f;
		m_score++;
	}
	else
	{
		m_emitterDropExplosion->pos = _pos;
		m_emitterDropExplosion->rate = 0.5f;
		m_life--;
		if (m_life == 0)
		{
			m_overTime = m_stateTimer;
			ToState(GameState::Over);
		}
	}
}

XMFLOAT2 App::GetPositionFromTrigo(const float& _angle, const float& _radius)
{
	float _sin = XMScalarSin(_angle);
	float _cos = XMScalarCos(_angle);
	return XMFLOAT2(_sin * _radius, _cos * _radius);
}

XMFLOAT3 App::RandXYPos(const int& _max)
{
	int _x1 = rand() % _max;
	int _x1Sign = rand() % 2 == 0 ? -1 : 1;
	int _y1 = rand() % _max;
	int _y1Sign = rand() % 2 == 0 ? -1 : 1;

	return XMFLOAT3((float)_x1 * (float)_x1Sign, 0.f, (float)_y1 * (float)_y1Sign);
}

void App::MyPixelShader(cpu_ps_io& io)
{
	// YOUR CODE HERE
	io.color = io.p.color;
}
