#include "Engine.h"

using namespace std;


void PrintToScreen(std::string str) 
{
	#ifdef _WIN32 // WINDOWS
		cout << str << endl;
	#elif __unix__ // UNIX
		printw("%s\n", str.c_str());
	#endif
}


Component::~Component()
{
}

void Component::SetActor(Actor* actor)
{
	_actor = actor;
}

void MoveComponent::Tick()
{
	Pawn* pawn = dynamic_cast<Pawn*>(_actor);
	InputManager& input = InputManager::GetInstance();

	if (pawn)
	{
		Vector2 movement;
		if (input.IsKeyFirstPressed('W'))
		{
			movement.y += 1.0f;
		}
		if (input.IsKeyFirstPressed('A'))
		{
			movement.x += 1.0f;
		}
		if (input.IsKeyFirstPressed('S'))
		{
			movement.y += -1.0f;
		}
		if (input.IsKeyFirstPressed('D'))
		{
			movement.x += -1.0f;
		}

		pawn->ApplyMovement(movement);
	}
}

Actor::Actor()
{}

Actor::~Actor()
{
	for (Component* component : _components)
	{
		delete component;
	}
}

void Actor::AddComponent(Component* component)
{
	component->SetActor(this);
	_components.push_back(component);
}

Component* Actor::GetComponent(ComponentTypeID ct) const
{
	for (Component* component : _components)
	{
		if (component->GetComponentTypeID() == ct)
		{
			return component;;
		}
	}

	return nullptr;
}

void Actor::Tick()
{
	for (Component* component : _components)
	{
		component->Tick();
	}
}

void Actor::SetAttribute(string name, int val)
{
	auto iter = _attributes.find(name);
	if (iter == _attributes.end())
	{
		_attributes.insert(std::pair<string, int>(name, val));
	}
	else
	{
		iter->second = val;
	}
}

bool Actor::TryGetAttribute(string name, int& val) const
{
	auto iter = _attributes.find(name);
	if (iter == _attributes.end())
	{
		return false;
	}
	val = iter->second;
	return true;
}

World* Actor::GetWorld() const
{
	return _world;
}

void Actor::SetWorld(World* world)
{
	_world = world;
}


Pawn::Pawn()
{}

void Pawn::ApplyMovement(Vector2 movement)
{
	_velocity.Add(movement);
}

void Pawn::Tick()
{
	Actor::Tick();
	if (_velocity.x != 0 || _velocity.y != 0)
	{
		_frame.pos.Add(_velocity);
		_frame.facing = _velocity;
	}

	memset(&_velocity, 0, sizeof(_velocity));
}

Frame Pawn::GetFrame()
{
	return _frame;
}

InputManager::InputManager()
	: _isAnyKeyFirstPressed(false)
{
	_keyMap = new SHORT[NUM_KEYS];
	#ifdef __unix__
		_pressedKeys = new SHORT[NUM_KEYS];
	#endif
}

InputManager::~InputManager()
{
	delete[] _keyMap;
}

InputManager& InputManager::GetInstance()
{
	static InputManager im;
	return im;
}

void InputManager::CaptureInput()
{
	_isAnyKeyFirstPressed = false;
	// WINDOWS
	#ifdef _WIN32
		for (int key = 0; key < NUM_KEYS; ++key)
		{
			SHORT curState = GetAsyncKeyState(key);
			SHORT prevState = _keyMap[key];

			curState &= ~MASK_KEYDOWN;
			if (prevState == 0 &&
				(curState & MASK_KEYPRESSED) == MASK_KEYPRESSED)
			{
				curState |= MASK_KEYDOWN;
				_isAnyKeyFirstPressed = true;
			}

			_keyMap[key] = curState;
		}
	// UNIX
	#elif __unix__
		/**
		 * Recreate GetAsyncKeyState functionality to minimize changes
		 * to rest of code. 
		 */
		int ch;
		while ((ch = getch()) != ERR)
		{
			_pressedKeys[toupper(ch)] = MASK_KEYPRESSED;
		}

		for (int key = 0; key < NUM_KEYS; ++key)
		{
			SHORT curState = _pressedKeys[key];
			SHORT prevState = _keyMap[key];

			curState &= ~MASK_KEYDOWN;
			if (prevState == 0 &&
				(curState & MASK_KEYPRESSED) == MASK_KEYPRESSED)
			{
				curState |= MASK_KEYDOWN;
				_isAnyKeyFirstPressed = true;
			}
			else if (prevState != 0)
			{
				curState |= prevState>>4;
			}

			_keyMap[key] = curState;
			_pressedKeys[key] = 0;
		}
	#endif
}

bool InputManager::IsKeyFirstPressed(int key) const
{
	//printw("iskey %i", _keyMap[key]);
	return (_keyMap[key] & MASK_KEYDOWN) == MASK_KEYDOWN;
}

bool InputManager::IsKeyPressed(int key) const
{
	return (_keyMap[key] & MASK_KEYPRESSED) == MASK_KEYPRESSED;
}

bool InputManager::IsAnyKeyFirstPressed() const
{
	return _isAnyKeyFirstPressed;
}

World::World()
{
}

World::~World()
{
	for (Actor* actor : _actors)
	{
		delete actor;
	}
}

void World::Tick()
{
	for (Actor* actor : _actors)
	{
		actor->Tick();
	}
}

