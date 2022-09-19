#pragma once

#include "Engine.h"

/** 
* An actor that can be bounded to an Area, [0, max)]
*/
class ActorHuntBoundablePawn : public Pawn
{
public:
	ActorHuntBoundablePawn();
	virtual void Tick() override;

	/** Turns on and sets bounds checking [0, width), [0, height)*/
	void setBounds(int width, int height);

private:
	bool _bounded;
	float _maxWidth;
	float _maxHeight;
};


/**
 * Player Pawn 
 */
class ActorHuntPlayerPawn : public ActorHuntBoundablePawn
{
public:
	ActorHuntPlayerPawn();
	virtual void Tick() override;

	void TakeDamage();
	void TakeHealing();

	/** Returns true if game over and prints message */
	bool GameOver() const;

private:
	int _hp;
	int _points;
};


class ActorHuntDamagingPawn : public ActorHuntBoundablePawn
{
public:
	ActorHuntDamagingPawn();
	virtual void Tick() override;
};


class ActorHuntHealingPawn : public ActorHuntBoundablePawn
{
public:
	ActorHuntHealingPawn();
	virtual void Tick() override;
};