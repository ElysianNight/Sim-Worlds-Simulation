#include "game.h"
#include "gameobject.h"
#include "GameObject2D.h"
#include "ObjectList.h"
#include <windows.h>
#include "CommonStates.h"
#include "gamedata.h"
#include <time.h>
#include "helper.h"
#include "DDSTextureLoader.h"
#include "drawdata.h"
#include "DrawData2D.h"
#include <stdlib.h>

using namespace DirectX;

extern HWND g_hWnd;

Game::Game(ID3D11Device* _pd3dDevice, HINSTANCE _hInstance) :m_playTime(0), m_myEF(nullptr)
{
	/* initialize random seed: */
	srand(time(NULL));

	m_pKeyboard = nullptr;
	m_pDirectInput = nullptr;

	//set up DirectXTK Effects system
	m_myEF  = new MyEffectFactory(_pd3dDevice);
#if DEBUG
	m_myEF->SetPath(L"Debug\\");
#else
	m_myEF->SetPath(L"Release\\");
#endif

	
	// Create other render resources here
	m_States=new DirectX::CommonStates(_pd3dDevice);

	//Direct Input Stuff
	HRESULT hr = DirectInput8Create(_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, NULL);
	hr = m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, NULL);
	hr = m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = m_pKeyboard->SetCooperativeLevel(g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	// initialize the mouse
	hr = m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouse, NULL);
	hr = m_pMouse->SetCooperativeLevel(g_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	hr = m_pMouse->SetDataFormat(&c_dfDIMouse);

	m_GD = new GameData();
	m_GD->keyboard = m_keyboardState;
	m_GD->prevKeyboard = m_prevKeyboardState;
	m_GD->mouse = &m_mouse_state;
	m_GD->GS = GS_PLAY_MAIN_CAM;

	//create a base camera
	m_cam = new Camera(0.25f * XM_PI, 640.0f / 480.0f, 1.0f, 10000.0f, Vector3::Zero, Vector3::UnitY);
	m_cam->SetPos( Vector3(0.0f, 750.0f, 750.0f) );
	m_GameObjects.push_back(m_cam);

/*	Terrain* terrain = new Terrain("table.cmo", _pd3dDevice, m_myEF,Vector3(100.0f,0.0f,100.0f), 0.0f, 0.0f, 0.0f, 0.25f * Vector3::One);
	m_GameObjects.push_back(terrain); */

	//Create new turret base game object... 
	//...in this case, an eagle
	Turret_Base* base = new Turret_Base("PredatorModel.cmo", _pd3dDevice, m_myEF);
	m_GameObjects.push_back(base);
	base->SetPos(Vector3(100.0f, -10.0f, -100.0f));

	m_TPSCam = new TPSCamera(0.25f * XM_PI, 640.0f / 480.0f, 1.0f, 10000.0f, base, Vector3::UnitY, Vector3(-200.0f, 100.0f, 0.0f));
	m_GameObjects.push_back(m_TPSCam);
	
	m_Light = new Light(Vector3(0.0f, 100.0f, 160.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.4f, 0.1f, 0.1f, 1.0f));
	m_GameObjects.push_back(m_Light);

/*	FileVBGO* terrainBox = new FileVBGO("../Assets/terrainTex.txt", _pd3dDevice);
	m_GameObjects.push_back(terrainBox);

	FileVBGO* Box = new FileVBGO("../Assets/cube.txt", _pd3dDevice);
	m_GameObjects.push_back(Box);
	Box->SetPos(Vector3(0.0f, 0.0f, -100.0f));
	Box->SetPitch( XM_PIDIV4 );
	Box->SetScale( 20.0f ); */

	for (int i = 0; i < 201; i++)
	{
			VBCube* cube = new VBCube();
			cube->m_alive = true;
			cube->init(11, _pd3dDevice);
			cube->SetPos(Vector3(rand() % 500 - 250, 0.0f, rand() % 500 - 250));
			cube->SetYaw(2.0f*3.141f*(float)rand() / (float)RAND_MAX);
			cube->SetScale(2.0f);
			m_GameObjects_boids.push_back(cube);
	}

	for (int i = 0; i < 301; i++)
	{
		VBCube* cube = new VBCube();
		cube->m_alive = false;
		cube->init(11, _pd3dDevice);
		cube->SetPos(Vector3(rand() % 500 - 250, 0.0f, rand() % 500 - 250));
		cube->SetYaw(2.0f*3.141f*(float)rand() / (float)RAND_MAX);
		cube->SetScale(2.0f);
		m_GameObjects_boids.push_back(cube);
	}

/*	SpikedVB* spikes = new SpikedVB();
	spikes->init(11, _pd3dDevice);
	spikes->SetPos(Vector3(0.0f, 0.0f, 100.0f));
	spikes->SetScale(4.0f);
	m_GameObjects.push_back(spikes);

	Spiral* spiral = new Spiral();
	spiral->init(11, _pd3dDevice);
	spiral->SetPos(Vector3(-100.0f, 0.0f, 0.0f));
	spiral->SetScale(4.0f);
	m_GameObjects.push_back(spiral);

	Pillow* pillow = new Pillow();
	pillow->init(11, _pd3dDevice);
	pillow->SetPos( Vector3(-100.0f, 0.0f, -100.0f) );
	pillow->SetScale(4.0f);
	m_GameObjects.push_back(pillow);

	Snail* snail = new Snail(_pd3dDevice, "../Assets/baseline.txt", 150, 0.98, 0.09f * XM_PI , 0.4f, Color(1.0f, 0.0f, 0.0f, 1.0f), Color(0.0f, 0.0f, 1.0f, 1.0f));
	snail->SetPos(Vector3(-100.0f, 0.0f, 100.0f));
	snail->SetScale(2.0f);
	m_GameObjects.push_back(snail);

	GameObject2D* logo = new GameObject2D("logo", _pd3dDevice);
	logo->SetPos(200.0f * Vector2::One);
	m_GameObject2Ds.push_back(logo); */
	
	ID3D11DeviceContext* pd3dImmediateContext;
	_pd3dDevice->GetImmediateContext(&pd3dImmediateContext);
	
	// Create DirectXTK spritebatch stuff
	m_DD2D = new DrawData2D();
	m_DD2D->m_Sprites.reset(new SpriteBatch(pd3dImmediateContext));
	m_DD2D->m_Font.reset(new SpriteFont(_pd3dDevice, L"italic.spritefont"));
		
	//create Draw Data
	m_DD = new DrawData();
	m_DD->pd3dImmediateContext = pd3dImmediateContext;
	m_DD->states = m_States;
	m_DD->light = m_Light;

	//initilise the defaults for the VBGOs
	VBGO::Init(_pd3dDevice);

}

Game::~Game()
{
	VBGO::CleanUp();

	//tidy away Direct Input Stuff
	if (m_pMouse)
	{
		m_pMouse->Unacquire();
		m_pMouse->Release();
	}
	if (m_pKeyboard)
	{
		m_pKeyboard->Unacquire();
		m_pKeyboard->Release();
	}
	if (m_pDirectInput) m_pDirectInput->Release();


	//get rid of the game objects here
	for (list<GameObject *>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		delete (*it);
	}
	for (list<GameObject *>::iterator it = m_GameObjects_boids.begin(); it != m_GameObjects_boids.end(); it++)
	{
		delete (*it);
	}

	m_GameObjects.clear();
	//and the 2D ones
	for (list<GameObject2D *>::iterator it = m_GameObject2Ds.begin(); it != m_GameObject2Ds.end(); it++)
	{
		delete (*it);
	}

	m_GameObject2Ds.clear();

	delete m_States;
	delete m_myEF;
	delete m_GD;
	delete m_DD;
}

bool Game::update()
{
	ReadKeyboard();
	ReadMouse();

	if (m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return false;
	}

	if (m_mouse_state.rgbButtons[2] & 0x80)
	{
		return false;
	}

	
	if ((m_keyboardState[DIK_SPACE] & 0x80) && !(m_prevKeyboardState[DIK_SPACE] & 0x80))
	{
		if (m_GD->GS == GS_PLAY_MAIN_CAM)
		{
			m_GD->GS = GS_PLAY_TPS_CAM;
		}
		else
		{
			m_GD->GS = GS_PLAY_MAIN_CAM;
		}
	}

	//Increasing boid speed...
	if ((m_keyboardState[DIK_I] & 0x80) && !(m_prevKeyboardState[DIK_I] & 0x80))
	{
		GameObject::ms_speed += 0.1f;
	}

	//Decreasing boid speed...
	if((m_keyboardState[DIK_O] & 0x80) && !(m_prevKeyboardState[DIK_O] & 0x80))
	{ 
		GameObject::ms_speed -= 0.1f;
	}

	//Increasing predator speed...
	if((m_keyboardState[DIK_J] & 0x80) && !(m_prevKeyboardState[DIK_J] & 0x80))
	{
		GameObject::mp_speed += 10.0f;
	}

	//Decreasing predator speed...
	if ((m_keyboardState[DIK_K] & 0x80) && !(m_prevKeyboardState[DIK_K] & 0x80))
	{
		GameObject::mp_speed -= 10.0f;
	}

	//Adding boids...
	if ((m_keyboardState[DIK_N] & 0x80) && !(m_prevKeyboardState[DIK_N] & 0x80))
	{
		//Add 10 boids;
		int newBoids = 0;
		for (list<GameObject *>::iterator it = m_GameObjects_boids.begin(); it != m_GameObjects_boids.end(); it++)
		{
			if (!(*it)->m_alive)
			{
				(*it)->m_alive = 1;
				newBoids++;
			}
			if (newBoids == 10)
			{
				break;
			}
		}
	}

	//Removing 10 boids...
	if ((m_keyboardState[DIK_M] & 0x80) && !(m_prevKeyboardState[DIK_M] & 0x80))
	{
		int newBoids = 0;
		for (list<GameObject *>::iterator it = m_GameObjects_boids.begin(); it != m_GameObjects_boids.end(); it++)
		{
			if ((*it)->m_alive)
			{
				(*it)->m_alive = 0;
				newBoids++;
			}
			if (newBoids == 10)
			{
				break;
			}
		}
	}

	//calculate frame time-step dt for passing down to game objects
	DWORD currentTime = GetTickCount();
	m_GD->dt = min((float)(currentTime - m_playTime) / 1000.0f, 0.1f);
	m_playTime = currentTime;

	m_GD->avePos = Vector3::Zero;
	int countAlive = 0;

	//update all objects
	for (list<GameObject *>::iterator it = m_GameObjects_boids.begin(); it != m_GameObjects_boids.end(); it++)
	{
		if ((*it)->m_alive)
		{
			//Count how many boids are alive
			countAlive++;
			//Add up all of their positions
			m_GD->avePos += (*it)->GetPos();
		}

		for (list<GameObject *>::iterator it2 = it; it2 != m_GameObjects_boids.end(); it2++)
		{
			if (it != it2  && (*it)->m_type == OT_BOID && (*it2)->m_type==OT_BOID)
			{
				//update forces on it and it2
				(*it)->SetForces((*it2));
				(*it2)->SetForces((*it));
			}
		}
	}

	//Divide average position by the number of alive boids
	m_GD->avePos /= countAlive;

	for (list<GameObject *>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		(*it)->Tick(m_GD);//apply forces
	}
	for (list<GameObject *>::iterator it = m_GameObjects_boids.begin(); it != m_GameObjects_boids.end(); it++)
	{
		(*it)->Tick(m_GD);//apply forces
	}
	for (list<GameObject2D *>::iterator it = m_GameObject2Ds.begin(); it != m_GameObject2Ds.end(); it++)
	{
		(*it)->tick(m_GD);
	}

	return true;
}

void Game::render(ID3D11DeviceContext* _pd3dImmediateContext)
{
	m_DD->pd3dImmediateContext = _pd3dImmediateContext;

	m_DD->cam = m_cam;
	if (m_GD->GS == GS_PLAY_MAIN_CAM)
	{
		m_DD->cam = m_cam;
	}
	else if (m_GD->GS == GS_PLAY_TPS_CAM)
	{
		m_DD->cam = m_TPSCam;
	}
	
	VBGO::UpdateConstantBuffer(m_DD);

	//draw all 3D objects
	for (list<GameObject *>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		(*it)->Draw(m_DD);
	}
	for (list<GameObject *>::iterator it = m_GameObjects_boids.begin(); it != m_GameObjects_boids.end(); it++)
	{
		(*it)->Draw(m_DD);
	}
	
	/*
	This was the original code for printing the test message:...
	
	char number[10];
	itoa(10, number, 10); 
	string attempt = "Test Message " + string(number);

	...But I created my own string vectors to do this below
	*/

	//Setting up a vector (admittedly with only one entry) containing the application's title
	vector<string>title;
	title.push_back("Alex's Boid Simulation");

	//Setting up a vector (again, it only contains one entry) to hold the player's score
	
	/*
	...do something with the score here...
	...needs to add 100 points to the score every time a boid is eaten...
	...the boid GOs also need to be destroyed on player impact...
	*/
	
	//Setting up a vector containing the HUD elements
	vector<string>attempts;
	attempts.push_back("Press I to increase boid speed or O to decrease boid speed");
	attempts.push_back("Press J to increase predator speed or K to decrease predator speed");
	attempts.push_back("Press N to add 10 boids or M to remove 10 boids");
	
	//Display the current values of the prey speed, predator speed and number of boids
	/*
	I couldn't work out how to do the string stream stuff...
	...to display the current values of the different user-altered parameters.
	*/
	
	// Draw sprite batch stuff
	m_DD2D->m_Sprites->Begin();

	for (list<GameObject2D *>::iterator it = m_GameObject2Ds.begin(); it != m_GameObject2Ds.end(); it++)
	{
		(*it)->draw(m_DD2D);
	}

	//Print the application's title to the screen
	for (int i = 0; i < title.size(); i++)
	{
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(title[i].c_str()), Vector2(600, 10), Colors::Black, 0.0f, Vector2::One, Vector3(1.0f, 1.0f, 1.0f));
	}

	//Print the HUD to the screen
	for (int i = 0; i < attempts.size(); i++)
	{
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(attempts[i].c_str()), Vector2(50, 90 + (i * 30)), Colors::White, 0.0f, Vector2::One, Vector3(0.6f, 0.6f, 0.6f));
	}

	//End sprite batch drawing
	m_DD2D->m_Sprites->End();

	_pd3dImmediateContext->OMSetDepthStencilState(m_States->DepthDefault(), 0);

}

bool Game::ReadKeyboard()
{
	//copy over old keyboard state
	memcpy(m_prevKeyboardState, m_keyboardState, sizeof(m_keyboardState));

	//clear out previous state
	ZeroMemory(&m_keyboardState, sizeof(m_keyboardState));

	// Read the keyboard device.
	HRESULT hr = m_pKeyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(hr))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			m_pKeyboard->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;

}

bool Game::ReadMouse()
{
	//clear out previous state
	ZeroMemory(&m_mouse_state, sizeof(m_mouse_state));

	// Read the Mouse device.
	HRESULT hr = m_pMouse->GetDeviceState(sizeof(m_mouse_state), (LPVOID)&m_mouse_state);
	if (FAILED(hr))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			m_pMouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;

}
