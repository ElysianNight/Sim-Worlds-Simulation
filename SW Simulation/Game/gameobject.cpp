#include "gameobject.h"
#include "gamedata.h"
#include "ObjectType.h"

using namespace std;

GameObject::GameObject()
{
	//set the Gameobject to the origin with no rotation and unit scaling 
	m_pos = Vector3::Zero;
	m_vel = Vector3::Zero;
	m_acc = Vector3::Zero;
	m_pitch = 0.0f;
	m_yaw = 0.0f;
	m_roll = 0.0f;
	m_scale = Vector3::One;

	m_worldMat = Matrix::Identity;
	m_fudge = Matrix::Identity;
	m_drag = 5.0f;

	m_type = OT_NULL;
}

GameObject::~GameObject()
{

}

void GameObject::SetForces(GameObject* _GO)
{
	//dpos vector from me to the other guy
	Vector3 dPos = _GO->m_pos - m_pos;

	//normalise that vector
	dPos.Normalize();

	float Length = dPos.Length();

	if (Length < 10.0f)
	{
		//apply acc along that vector scaled by how much I want to be pulled over to that guy
		m_acc -= 0.75f * dPos;
	}
	else if (Length > 10.0f && Length < 50.0f)
	{
		//apply acc along that vector scaled by how much I want to be pulled over to that guy
		m_acc += 0.75f * dPos;
	}
}

void GameObject::Tick(GameData* _GD)
{
	Vector3 newPos = m_pos + _GD->dt * m_vel;
	Vector3 newVel = m_vel + _GD->dt * (m_acc  - m_drag * m_vel);
	
	m_pos = newPos;
	m_vel = newVel;
	m_acc = Vector3::Zero;
	
	//build up the world matrix depending on the new position of the GameObject
	//the assumption is that this class will be inherited by the class that ACTUALLY changes this
	Matrix  scaleMat	= Matrix::CreateScale( m_scale);
	m_rotMat = Matrix::CreateFromYawPitchRoll(m_yaw, m_pitch, m_roll);//possible not the best way of doing this!
	Matrix  transMat = Matrix::CreateTranslation(m_pos);

	m_worldMat = scaleMat *m_rotMat * transMat;
}