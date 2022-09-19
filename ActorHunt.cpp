#pragma once
#include <iostream>
#include <cstdlib>

#include "ActorHunt.h"

using namespace std;

ActorHuntBoundablePawn::ActorHuntBoundablePawn()
: _bounded(false)
{}

void ActorHuntBoundablePawn::Tick()
{
	Actor::Tick();
	if (_velocity.x != 0 || _velocity.y != 0)
	{
		_frame.facing = _velocity;

		if (_bounded)
		{
			if (_frame.pos.x + _velocity.x < 0)
			{
				_velocity.x = -_frame.pos.x;
			}
			else if (_frame.pos.x + _velocity.x >= _maxWidth)
			{
				_velocity.x = _maxWidth-1.0f - _frame.pos.x;
			}

			if (_frame.pos.y + _velocity.y < 0)
			{
				_velocity.y = -_frame.pos.y;
			}
			else if (_frame.pos.y + _velocity.y >= _maxHeight)
			{
				_velocity.y = _maxHeight-1.0f - _frame.pos.y;
			}
		}

		_frame.pos.Add(_velocity);
	}

	memset(&_velocity, 0, sizeof(_velocity));
}

void ActorHuntBoundablePawn::setBounds(int width, int height)
{
	_bounded = true;
	_maxWidth = (float)width;
	_maxHeight = (float)height;
}


ActorHuntPlayerPawn::ActorHuntPlayerPawn() 
: _hp(100), _points(0)
{}

void ActorHuntPlayerPawn::Tick()
{
	ActorHuntBoundablePawn::Tick();

	InputManager& input = InputManager::GetInstance();

	int accruedDamage = 0;

	// simulate damage
	if (input.IsKeyFirstPressed('1'))
	{
		accruedDamage += 10;
	}
	if (input.IsKeyFirstPressed('2'))
	{
		accruedDamage += 20;
	}

	if (accruedDamage >= 20)
	{
		PrintToScreen(" player Took a critical hit!");
	}

	_hp -= accruedDamage;
	if (_hp < 0)
	{
		_hp = 0;
	} 

	if (accruedDamage != 0)
	{
		PrintToScreen( " player is now at " + to_string(_hp) +  " hitpoints.");
	}
}

void ActorHuntPlayerPawn::TakeDamage()
{
	_hp -= 20;
	_points += 10;

	if (_hp < 0)
	{
		_hp = 0;
	} 

	string msg = " player is now at " + to_string(_hp) +
								" hitpoints and " + to_string(_points) + "points";
	PrintToScreen(msg);
}

void ActorHuntPlayerPawn::TakeHealing()
{
	_hp += 10;
	_points += 5;

	if (_hp >= 100)
	{
		_hp = 100;
	}

	string msg = " player is now at " + to_string(_hp) +
								" hitpoints and " + to_string(_points) + "points";
	PrintToScreen(msg);
}

bool ActorHuntPlayerPawn::GameOver() const
{
	if (_hp < 1)
	{
		PrintToScreen(" GAME OVER! YOU LOSE :(");
		return true;
	}

	if (_points >= 100)
	{
		PrintToScreen(" GAME OVER! YOU WIN :)");
		return true;
	}

	return false;
}


ActorHuntDamagingPawn::ActorHuntDamagingPawn()
{}

void ActorHuntDamagingPawn::Tick()
{
	ActorHuntBoundablePawn::Tick();

	// random ints > 3 mean don't move
	int randomMove = rand() % 6;
	float moveSpeed = 1.0f;

	Vector2 movement;
	if (randomMove == 0)
	{
		movement.y += moveSpeed;
	}
	if (randomMove == 2)
	{
		movement.x += moveSpeed;
	}
	if (randomMove == 2)
	{
		movement.y += -moveSpeed;
	}
	if (randomMove == 3)
	{
		movement.x += -moveSpeed;
	}

	this->ApplyMovement(movement);
}


ActorHuntHealingPawn::ActorHuntHealingPawn()
{}

void ActorHuntHealingPawn::Tick()
{
	ActorHuntBoundablePawn::Tick();

	// random ints > 3 mean don't move
	int randomMove = rand() % 10;
	float moveSpeed = 1.0f;

	Vector2 movement;
	if (randomMove == 0)
	{
		movement.y += moveSpeed;
	}
	if (randomMove == 1)
	{
		movement.x += moveSpeed;
	}
	if (randomMove == 2)
	{
		movement.y += -moveSpeed;
	}
	if (randomMove == 3)
	{
		movement.x += -moveSpeed;
	}

	this->ApplyMovement(movement);
}