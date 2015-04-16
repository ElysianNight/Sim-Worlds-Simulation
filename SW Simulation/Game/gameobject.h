#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H
//Base Game Object Class
#include "SimpleMath.h"
#include "CommonStates.h"
#include "ObjectType.h"

using namespace DirectX;
using namespace SimpleMath;

class Camera;
struct GameData;
struct DrawData;

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

	//Object type variable
	ObjectType m_type;

	//Initialising alive/dead state
	bool m_alive = 1;

	virtual void Tick(GameData* _GD );
	virtual void Draw(DrawData* _DD) = 0;

	Vector3 GetPos(){ return m_pos; }
	void SetPos(Vector3 _pos){ m_pos = _pos; }

	float GetPitch(){ return m_pitch; }
	float GetYaw(){ return m_yaw; }
	float GetRoll(){ return m_roll; }

	void SetPitch(float _pitch){ m_pitch = _pitch; }
	void SetYaw(float _yaw){ m_yaw = _yaw; }
	void SetRoll(float _roll){ m_roll = _roll; }
	void SetPitchYawRoll(float _pitch, float _yaw, float _roll)
	{ 
		m_pitch = _pitch; m_yaw = _yaw; m_roll = _roll; 
	}

	void SetScale(Vector3 _scale){ m_scale = _scale; }
	void SetScale(float _scale){ m_scale = _scale * Vector3::One; }
	
	
	void SetForces(GameObject* _GO);
	//Boid speed
	static float ms_speed;
	//Predator speed
	static float mp_speed;

protected:
	//Initialising the initial values of vectors to 0
	Vector3 m_pos = Vector3::Zero;
	Vector3 m_vel = Vector3::Zero;
	Vector3 m_acc = Vector3::Zero;
	float m_pitch, m_yaw, m_roll;
	Vector3 m_scale;

	Matrix m_worldMat;
	Matrix m_rotMat;
	Matrix m_fudge;

	//Initialising drag force
	float m_drag = 0;
};

#endif