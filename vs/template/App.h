#pragma once

class Catcher;
class Decor;
class Drop;
class DropFactory;
class DropManager;

class App
{
	enum GameState
	{
		Start,
		Engage,
		Game,
		Over
	};

private:
	inline static App* s_pApp = nullptr;

	cpu_font m_font;
	cpu_font m_titlefont;
	cpu_mesh m_meshCircle1;
	cpu_mesh m_meshCircle2;
	cpu_mesh m_meshRail;
	cpu_mesh m_meshCatcher;

	cpu_particle_emitter* m_dropSpawn;
	cpu_particle_emitter* m_dropExplosion;
	cpu_particle_emitter* m_dropCatch;

	Catcher* m_catcher;
	int m_score;
	int m_life;
	float m_time;

	GameState m_gameState;
	float m_stateTimer;
	float m_startTime;
	float m_startOffset;
	float m_overTime;

	Decor* m_circle1;
	Decor* m_circle2;
	Decor* m_rail;
	float m_railRadius;

	DropManager* m_dropManager;
	DropFactory* m_dropFactory;
	float m_dropTimer;
	float m_dropTime;

public:
	App();
	virtual ~App();

	static App& GetInstance() { return *s_pApp; }
	DropManager* GetDropManager() { return m_dropManager; }

	void OnStart();
	void OnUpdate();
	void OnExit();
	void OnRender(int pass);

	void StartUpdate();
	void EngageUpdate();
	void GameUpdate();
	void OverUpdate();
	void ToState(GameState _state);
	void UpdateCameraPosition();
	void DropExplosion(const XMFLOAT3& _pos, const bool _isCatch);

	XMFLOAT2 GetPositionFromTrigo(const float& _angle, const float& _radius);
	XMFLOAT3 RandXYPos(const int& _max);

	static void MyPixelShader(cpu_ps_io& io);
};

// Add State : launch, engage

