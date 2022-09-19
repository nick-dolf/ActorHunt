#include <iostream>
#include <cstdlib>
#include <ctime>

/**
* Handle different OS Headers
*/
#ifdef _WIN32 // WINDOWS
  #include <Windows.h>
#elif __unix__ // UNIX
  #include <ncurses.h>
	#include <unistd.h>
#endif

#include "ActorHunt.h"
using namespace std;


/**
* Clears screen for different OS' 
*/
void ClearScreen(char fill = ' ')
{
	#ifdef _WIN32 // WINDOWS
		const COORD tl = { 0,0 };
		CONSOLE_SCREEN_BUFFER_INFO s;
		const HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo(console, &s);
		DWORD written, cells = s.dwSize.X * s.dwSize.Y;
		FillConsoleOutputCharacter(console, fill, cells, tl, &written);
		FillConsoleOutputAttribute(console, s.wAttributes, cells, tl, &written);
		SetConsoleCursorPosition(console, tl);
	#elif __unix__ // UNIX
		clear();
	#endif
}

int main()
{
  /**
  * Initialize NCurses for Unix
  */
  #if __unix__
		initscr();
		cbreak();
		noecho();
		nodelay(stdscr, true);
		halfdelay(1);
  #endif
  
	srand((int) time(0));

	const int WORLD_HEIGHT = 20;
	const int WORLD_WIDTH = 40;
	const int DAMAGE_ACTOR_NUM = 13;
	const int HEALING_ACTOR_NUM = 11;
	char worldMap[WORLD_HEIGHT][WORLD_WIDTH+1] = {0};

	World * world = new World();

	for (int i = 0; i < HEALING_ACTOR_NUM; ++i)
	{
		float x = (float)(rand() % WORLD_WIDTH-1);
		float y = (float)(rand() % WORLD_HEIGHT-1);
		ActorHuntHealingPawn * healingActor = world->CreateActor<ActorHuntHealingPawn>();
		healingActor->setBounds(WORLD_WIDTH, WORLD_HEIGHT);
		healingActor->ApplyMovement(Vector2(x, y));
	}

	for (int i = 0; i < DAMAGE_ACTOR_NUM; ++i)
	{
		float x = (float)(rand() % WORLD_WIDTH-1);
		float y = (float)(1 + rand() % WORLD_HEIGHT-1);
		ActorHuntDamagingPawn * damagingActor = world->CreateActor<ActorHuntDamagingPawn>();
		damagingActor->setBounds(WORLD_WIDTH, WORLD_HEIGHT);
		damagingActor->ApplyMovement(Vector2(x, y));
	}
	
	ActorHuntPlayerPawn * mainCharacter = world->CreateActor<ActorHuntPlayerPawn>();
	mainCharacter->AddComponent( new MoveComponent() );
	const Vector2 move(WORLD_WIDTH / 2.0f, WORLD_HEIGHT / 2.0f);
	mainCharacter->setBounds(WORLD_WIDTH, WORLD_HEIGHT);
	mainCharacter->ApplyMovement(move);

	bool firstFrame = true;

	while (true)
	{
		InputManager & input = InputManager::GetInstance();
		input.CaptureInput();

		if (input.IsAnyKeyFirstPressed())
		{
			ClearScreen();
		}

		if (input.IsKeyPressed('Q'))
		{
			PrintToScreen("Exiting simulation");
			break;
		}

		if (firstFrame)
		{
			string welcome = 
				"**Welcome to Actor Hunt**\n\n"
				"   Touch a 'D'angerous actor to get big points but watch out\n"
				"   Touch a 'H'ealing actor to get small points and a warm fuzzy feeling\n\n"
				"   Move 'V'ourself around with 'WASD' as is the stye around these parts\n\n"
				"   Press 'The Any Key'tm to start hunting\n"
				"   Except 'Q', that's only if you want to leave(you monster)\n";

			PrintToScreen(welcome);
			#ifdef __unix__
			refresh();
			#endif
			firstFrame = false;
		}

		if (input.IsAnyKeyFirstPressed())
		{		
			world->Tick();	
			for (int r = 0; r < WORLD_HEIGHT; ++r)
			{
				for (int c = 0; c < WORLD_WIDTH; ++c)
				{
					worldMap[r][c] = '#';
				}
			}

			for (Actor* actor : world->GetAllActors())
			{
				ActorHuntPlayerPawn* hero = dynamic_cast<ActorHuntPlayerPawn*>(actor);
				ActorHuntDamagingPawn* damage = dynamic_cast<ActorHuntDamagingPawn*>(actor);
				ActorHuntHealingPawn* healing = dynamic_cast<ActorHuntHealingPawn*>(actor);

				if (healing)
				{
					const Frame frame = healing->GetFrame();

					char sigil = 'H';
					
					int r = WORLD_HEIGHT-1 - (int)frame.pos.y;
					int c = WORLD_WIDTH-1  - (int)frame.pos.x;
					
					worldMap[r][c] = sigil;
				}

				else if (damage)
				{
					const Frame frame = damage->GetFrame();

					char sigil = 'D';

					int r = WORLD_HEIGHT-1 - (int)frame.pos.y;
					int c = WORLD_WIDTH-1  - (int)frame.pos.x ;
					
					worldMap[r][c] = sigil;
				}

				else if (hero)
				{
					const Frame frame = hero->GetFrame();

					char sigil = 'o';
					if (frame.facing.x > 0)
					{
						sigil = '<';
					}
					else if (frame.facing.x < 0)
					{
						sigil = '>';
					}

					if (frame.facing.y > 0)
					{
						sigil = '^';
					}
					else if (frame.facing.y < 0)
					{
						sigil = 'V';
					}

					int r = WORLD_HEIGHT-1 - (int)frame.pos.y;
					int c = WORLD_WIDTH-1  - (int)frame.pos.x;

					worldMap[r][c] = sigil;

					for (ActorHuntDamagingPawn* pawn : world->FindPawnsAtLocation<ActorHuntDamagingPawn>(frame.pos.x, frame.pos.y))
					{
						hero->TakeDamage();
					}

					for (ActorHuntHealingPawn* pawn : world->FindPawnsAtLocation<ActorHuntHealingPawn>(frame.pos.x, frame.pos.y))
					{
						hero->TakeHealing();
					}
				}
			}

			if (mainCharacter->GameOver())
			{
				break;
			}

			for (int r = 0; r < WORLD_HEIGHT; ++r)
			{
				PrintToScreen(worldMap[r]);
			}
			
			PrintToScreen("Move: WASD  Quit: Q");
		}
	}

	delete world;

	#ifdef __unix__
		PrintToScreen("(closing in 5s)");
		refresh();
		sleep(5);
		endwin();
	#endif

	return 0;
}
