#pragma once
#include "Game.h"
#include "PhysicsController.h"
#include "PhysicsFactory.h"

namespace BGE
{
	class AnimatAssignment :
		public Game
	{
	public:
		AnimatAssignment(void);

		float elapsed;
		int numberOfCreatures;
		bool floating;
		bool straightLegs;
		bool moveSwitch;
		bool Initialise();
		void creatureCreature();
		void Update();
		void Cleanup();
		//std::list<shared_ptr<PhysicsController>> octoHeads;
		//std::list<shared_ptr<PhysicsController>>::iterator headIt;
		shared_ptr<PhysicsController> octoHead;
		btHingeConstraint * testHinge;
		std::list<btHingeConstraint*> octoHinges;
		std::list<btHingeConstraint*>::iterator it;
		shared_ptr<PhysicsController> createCreature(int size, glm::vec3 position);
	};
}