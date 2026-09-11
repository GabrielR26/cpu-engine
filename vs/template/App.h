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

	cpu_particle_emitter* m_emitterDropSpawn;
	cpu_particle_emitter* m_emitterDropExplosion;
	cpu_particle_emitter* m_emitterDropCatch;

	Catcher* m_catcher;
	int m_score;
	int m_life;
	float m_time;

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
	float m_dropTimeMin;

	cpu_fsm<App>* m_FSM;

	std::string m_strGameOver;
	std::string m_strLife;
	std::string m_strScore;
	std::string m_strTime;
	std::string m_strStart;
	std::string m_strCommandLeft;
	std::string m_strCommandRight;
	std::string m_strCommandSpace;
	std::string m_strCommandEchap;

public:
	App();
	virtual ~App();

	static App& GetInstance() { return *s_pApp; }

	inline cpu_particle_emitter* GetEmitterDropSpawn() { return m_emitterDropSpawn; }
	inline cpu_particle_emitter* GetEmitterDropExplosion() { return m_emitterDropExplosion; }
	inline cpu_particle_emitter* GetEmitterDropCatch() { return m_emitterDropCatch; }
	inline Catcher* GetCatcher() { return m_catcher; }
	inline int GetScore() { return m_score; }
	inline int GetLife() { return m_life; }
	inline float GetTime() { return m_time; }
	inline float GetStartTime() { return m_startTime; }
	inline float GetStartOffset() { return m_startOffset; }
	inline float GetOverTime() { return m_overTime; }
	inline Decor* GetCircle1() { return m_circle1; }
	inline Decor* GetCircle2() { return m_circle2; }
	inline Decor* GetRail() { return m_rail; }
	inline float GetRailRadius() { return m_railRadius; }
	inline DropManager* GetDropManager() { return m_dropManager; }
	inline DropFactory* GetDropFactory() { return m_dropFactory; }
	inline float GetDropTimer() { return m_dropTimer; }
	inline float GetDropTime() { return m_dropTime; }
	inline float GetDropTimeMin() { return m_dropTimeMin; }
	inline cpu_fsm<App>* GetFSM() { return m_FSM; }

	void SetDropTime(const float& _time) { m_dropTime = _time; }
	void SetDropTimer(const float& _time) { m_dropTimer = _time; }
	void SetStrStart(const std::string& _str) { m_strStart = _str; }

	void OnStart();
	void OnUpdate();
	void OnExit();
	void OnRender(int pass);

	void UpdateCameraPosition();
	void DropExplosion(const XMFLOAT3& _pos, const bool _isCatch);

	XMFLOAT2 GetPositionFromTrigo(const float& _angle, const float& _radius);
	XMFLOAT3 RandXYPos(const int& _max);

	static void MyPixelShader(cpu_ps_io& io);
};

struct StateGameGlobal
{
	void OnEnter(App& cur, int from);
	void OnExecute(App& cur);
	void OnExit(App& cur, int to);
};

struct StateGameStart
{
	void OnEnter(App& cur, int from);
	void OnExecute(App& cur);
	void OnExit(App& cur, int to);
};

struct StateGamePlay
{
	void OnEnter(App& cur, int from);
	void OnExecute(App& cur);
	void OnExit(App& cur, int to);
};

struct StateGameOver
{
protected:
	float m_camAngle;

public:
	void OnEnter(App& cur, int from);
	void OnExecute(App& cur);
	void OnExit(App& cur, int to);
};