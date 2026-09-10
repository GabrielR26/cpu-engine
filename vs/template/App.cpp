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
	m_life = 100;
	m_startTime = 0.5f;
	m_dropTime = 1.5f;

	m_score = 0;
	m_time = 0.f;
	m_startOffset = 20.f;
	m_railRadius = 4.f;
	m_dropTimer = 0.f;
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

	m_strGameOver = "Game Over";
	m_strLife = "Life: ";
	m_strScore = "Score: ";
	m_strTime = "Time: ";
	m_strStart = "Ready ?";
	m_strCommandLeft = "Left => Go to left";
	m_strCommandRight = "Right => Go to right";
	m_strCommandSpace = "Space => jump overside";
	m_strCommandEchap = "Echap => Quit";

	m_FSM = cpuEngine.CreateFSM(this);
	m_FSM->SetGlobal<StateGameGlobal>();
	m_FSM->Add<StateGameStart>();
	m_FSM->Add<StateGamePlay>();
	m_FSM->Add<StateGameOver>();
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
	m_meshCircle1.CreateCircle(m_railRadius - 0.5f, 36, CPU_WHITE);
	for (int i = 0; i < 36 * 3; i += 12 * 3)
	{
		int _res = i / (12 * 3);
		XMFLOAT3 _color((_res == 0) * 0.5f, (_res == 1) * 0.5f, (_res == 2) * 0.5f);
		for (int j = 0; j < 12 * 3; j += 3)
		{
			m_meshCircle1.vertices[j + i].color = _color;
			m_meshCircle1.vertices[j + i + 1].color = _color;
			m_meshCircle1.vertices[j + i + 2].color = _color;
		}
	}
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
	m_emitterDropSpawn->colorMin = cpu::ToColor(255, 0, 0);
	m_emitterDropSpawn->colorMax = cpu::ToColor(255, 125, 0);
	m_emitterDropCatch = cpuEngine.CreateParticleEmitter();
	m_emitterDropCatch->rate = 0.1f;
	m_emitterDropCatch->spread = 1.0f;
	m_emitterDropCatch->durationMax = 1.0f;
	m_emitterDropCatch->colorMin = cpu::ToColor(0, 255, 0);
	m_emitterDropCatch->colorMax = cpu::ToColor(125, 255, 125);

	m_FSM->ToState(CPU_ID(StateGameStart));
	//OutputDebugStringA(std::to_string(m_dropManager->count).c_str());
}

void App::OnUpdate()
{
	// YOUR CODE HERE
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

	if (m_FSM->state == CPU_ID(StateGameStart))
	{
		cpuDevice.DrawText(&m_titlefont, m_strStart.c_str(), (int)(cpuDevice.GetWidth() * 0.5f), (int)(cpuDevice.GetHeight() * 0.2f), CPU_TEXT_CENTER, &_tintWhite);
	}
	else if (m_FSM->state == CPU_ID(StateGamePlay))
	{
		std::string _strLife = m_strLife + CPU_STR(m_life);
		std::string _strScore = m_strScore + CPU_STR(m_score);
		std::string _strTime = m_strTime + CPU_STR((int)m_FSM->totalTime);

		cpuDevice.DrawText(&m_font, _strLife.c_str(), (int)(cpuDevice.GetWidth() * 0.1f), 10, CPU_TEXT_CENTER, &_tintGreen);
		cpuDevice.DrawText(&m_font, _strScore.c_str(), (int)(cpuDevice.GetWidth() * 0.5f), 10, CPU_TEXT_CENTER, &_tintBlue);
		cpuDevice.DrawText(&m_font, _strTime.c_str(), (int)(cpuDevice.GetWidth() * 0.9f), 10, CPU_TEXT_CENTER, &_tintRed);
		cpuDevice.DrawText(&m_font, m_strCommandLeft.c_str(), (int)(cpuDevice.GetWidth() * 0.05f), (int)(cpuDevice.GetHeight() * 0.75f), CPU_TEXT_LEFT, &_tintBlack);
		cpuDevice.DrawText(&m_font, m_strCommandRight.c_str(), (int)(cpuDevice.GetWidth() * 0.05f), (int)(cpuDevice.GetHeight() * 0.80f), CPU_TEXT_LEFT, &_tintBlack);
		cpuDevice.DrawText(&m_font, m_strCommandSpace.c_str(), (int)(cpuDevice.GetWidth() * 0.05f), (int)(cpuDevice.GetHeight() * 0.85f), CPU_TEXT_LEFT, &_tintBlack);
		cpuDevice.DrawText(&m_font, m_strCommandEchap.c_str(), (int)(cpuDevice.GetWidth() * 0.05f), (int)(cpuDevice.GetHeight() * 0.90f), CPU_TEXT_LEFT, &_tintBlack);
	}
	else if (m_FSM->state == CPU_ID(StateGameOver))
	{
		std::string _strScore = m_strScore + CPU_STR(m_score);
		std::string _strTime = m_strTime + CPU_STR((int)m_overTime);

		cpuDevice.DrawText(&m_font, _strScore.c_str(), (int)(cpuDevice.GetWidth() * 0.3f), 10, CPU_TEXT_CENTER, &_tintBlue);
		cpuDevice.DrawText(&m_font, _strTime.c_str(), (int)(cpuDevice.GetWidth() * 0.7f), 10, CPU_TEXT_CENTER, &_tintRed);
		cpuDevice.DrawText(&m_titlefont, m_strGameOver.c_str(), (int)(cpuDevice.GetWidth() * 0.5f), (int)(cpuDevice.GetHeight() * 0.75f), CPU_TEXT_CENTER, &_tintWhite);
		cpuDevice.DrawText(&m_font, m_strCommandEchap.c_str(), (int)(cpuDevice.GetWidth() * 0.05f), (int)(cpuDevice.GetHeight() * 0.20f), CPU_TEXT_LEFT, &_tintBlack);
	}
}

void App::UpdateCameraPosition()
{
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
			m_overTime = m_FSM->totalTime;
			m_FSM->ToState(CPU_ID(StateGameOver));
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void StateGameGlobal::OnEnter(App& cur, int from)
{
}

void StateGameGlobal::OnExecute(App& cur)
{
	// Quit
	if (cpuInput.IsBackPressed())
		cpuEngine.Quit();
}

void StateGameGlobal::OnExit(App& cur, int to)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void StateGameStart::OnEnter(App& cur, int from)
{
}

void StateGameStart::OnExecute(App& cur)
{
	float _delta = cpu::Clamp(cur.GetFSM()->totalTime / cur.GetStartTime());
	float _catcherSpringArm = cur.GetCatcher()->GetSpringArm();

	// Zoom length
	float _camRadius = cpu::Lerp(cur.GetStartOffset(), cur.GetRailRadius() * _catcherSpringArm, _delta);
	// Zoom height
	float _camHeight = cpu::Lerp(_catcherSpringArm * 3, _catcherSpringArm, _delta);
	// Rotation
	float _camRad = cpu::Lerp(0.f, XM_2PI, _delta);
	XMFLOAT2 _camRot2 = cur.GetPositionFromTrigo(_camRad, _camRadius);
	/// Camera
	XMFLOAT3 _camRot3(_camRot2.x, _camHeight, _camRot2.y);
	cpuEngine.GetCamera()->transform.pos = _camRot3;
	cpuEngine.GetCamera()->transform.LookAt(0, 0, 0);

	if (cur.GetFSM()->totalTime < cur.GetStartTime())
	{
		/// Emitter
		cur.GetEmitterDropExplosion()->pos = cur.RandXYPos(10);
		cur.GetEmitterDropCatch()->pos = cur.RandXYPos(10);
		cur.GetEmitterDropSpawn()->pos = cur.RandXYPos(10);
	}

	if (cur.GetFSM()->totalTime >= cur.GetStartTime())
	{
		cur.SetStrStart("Go !!");
		cur.GetEmitterDropExplosion()->rate = 0.0f;
		cur.GetEmitterDropCatch()->rate = 0.0f;
		cur.GetEmitterDropSpawn()->rate = 0.0f;
	}

	if (cur.GetFSM()->totalTime >= cur.GetStartTime() + 0.75f)
		cur.GetFSM()->ToState(CPU_ID(StateGamePlay));
}

void StateGameStart::OnExit(App& cur, int to)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void StateGamePlay::OnEnter(App& cur, int from)
{
	cur.GetEmitterDropExplosion()->rate = 0.0f;
	cur.GetEmitterDropSpawn()->rate = 0.0f;
	cur.GetEmitterDropCatch()->rate = 0.0f;
	cur.GetEmitterDropCatch()->spread = 0.5f;
}

void StateGamePlay::OnExecute(App& cur)
{
	float _dt = cpuTime.delta;
	float _dropTime = cur.GetDropTime();
	cur.SetDropTime(cpu::Clamp(_dropTime -= _dt * _dt, 1.f, cur.GetDropTime()));

	// Disable rate for small explosion
	if (cur.GetEmitterDropExplosion()->rate > 0)
		cur.GetEmitterDropExplosion()->rate = 0.0f;
	if (cur.GetEmitterDropCatch()->rate > 0)
		cur.GetEmitterDropCatch()->rate = 0.0f;
	if (cur.GetEmitterDropSpawn()->rate > 0)
		cur.GetEmitterDropSpawn()->rate = 0.0f;

	if (cur.GetDropTimer() >= cur.GetDropTime())
	{
		Drop* _newDrop = cur.GetDropFactory()->SpawnDrop();
		cur.GetDropManager()->Add(_newDrop);
		cur.GetEmitterDropSpawn()->pos = _newDrop->GetTransform()->pos;
		cur.GetEmitterDropSpawn()->rate = 0.5f;
		cur.SetDropTimer(0.f);
	}
	else
		cur.SetDropTimer(cur.GetDropTimer() + _dt);

	// Catcher
	cur.GetCatcher()->Update(_dt);

	//// Drops
	for (size_t i = 0; i < cur.GetDropManager()->count; i++)
		cur.GetDropManager()->list[i]->Update(_dt);
	cur.GetDropManager()->Purge();
}

void StateGamePlay::OnExit(App& cur, int to)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void StateGameOver::OnEnter(App& cur, int from)
{
	cur.GetEmitterDropExplosion()->rate = 0.0f;
	cur.GetEmitterDropCatch()->rate = 0.0f;

	m_camAngle = cur.GetCatcher()->GetAngle();
}

void StateGameOver::OnExecute(App& cur)
{
	float _dt = cpuTime.delta;
	float _catcherSpringArm = cur.GetCatcher()->GetSpringArm();

	// Rotation
	m_camAngle += _dt;
	XMFLOAT2 _camRot2 = cur.GetPositionFromTrigo(m_camAngle, cur.GetRailRadius() * _catcherSpringArm);
	/// Camera
	XMFLOAT3 _camRot3(_camRot2.x, 3.f, _camRot2.y);
	cpuEngine.GetCamera()->transform.pos = _camRot3;
	cpuEngine.GetCamera()->transform.LookAt(0, 0, 0);
}

void StateGameOver::OnExit(App& cur, int to)
{
}