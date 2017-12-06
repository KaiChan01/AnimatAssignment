#include "AnimatAssignment.h"
#include "Utils.h"


using namespace BGE;

AnimatAssignment::AnimatAssignment(void)
{
}

bool AnimatAssignment::Initialise()
{
	float gravity = -1;
	floating = true;
	setGravity(glm::vec3(0, gravity, 0));
	
	physicsFactory->CreateGroundPhysics();
	physicsFactory->CreateCameraPhysics();
	
	numberOfCreatures = 1;
	numberOfObject = 10;
	/*	The idea of the for loop is that I can simulate many octopus in the BGE, but this requires very powerful processing
		My laptop cannot render more than 1 octopus
	
	for (int i = 0; i < numberOfCreatures; i++)
	{
		createCreature(rand() % 5 + 1, glm::vec3(rand() % -100 + 100, rand() % 50 + 20, rand() % -100 + 100));
	}
	*/
	
	generateScene(numberOfObject);
	createCreature(rand() % 5 + 1, glm::vec3(rand() % -100 + 100, rand() % 50 + 20, rand() % -100 + 100));

	elapsed = 10.0f;
	moveSwitch = true;
	Game::Initialise();
	return true;
}

void AnimatAssignment::Update()
{
	static float timeToMove = 1.5;

	if (elapsed > timeToMove)
	{
		/*
			This code is for rendering more than one octopus, but it's too expensive for my PC
			for (headIt = octoHeads.begin(); headIt != octoHeads.end(); headIt++)
			{
			shared_ptr<PhysicsController > octoHead = *headIt;
			octoHead->rigidBody->clearForces();
			}
		*/
		
		moveSwitch = !moveSwitch;
		elapsed = 0.0f;
	}

	for (it = octoHinges.begin(); it != octoHinges.end(); it++)
	{
		btHingeConstraint * updateHinge = *it;
		if (moveSwitch == true)
		{
			updateHinge->enableAngularMotor(true, 10, -10);

			/*
				For multiple octopus

				for (headIt = octoHeads.begin(); headIt != octoHeads.end(); headIt++)
				{
				shared_ptr<PhysicsController > octoHead = *headIt;
				if (floating == true)
				{
				octoHead->rigidBody->applyForce(btVector3(0, 45, 0), btVector3(0, -1, 0));
				}

				}
			*/
			
			if (floating == true)
			{
				octoHead->rigidBody->applyForce(btVector3(0, 45, 0), btVector3(0, -1, 0));
			}
		}
		else
		{
			updateHinge->enableAngularMotor(true, 10, 10);
			/* Multi octopus code

				for (headIt = octoHeads.begin(); headIt != octoHeads.end(); headIt++)
			{
				shared_ptr<PhysicsController > octoHead = *headIt;
				if (floating == true)
				{
					octoHead->rigidBody->applyForce(btVector3(0, -30, 0), btVector3(0, 1, 0));
					octoHead->rigidBody->applyGravity();
				}
			}
			*/
			
			if (floating == true)
			{
				octoHead->rigidBody->applyForce(btVector3(0, -30, 0), btVector3(0, 1, 0));
				octoHead->rigidBody->applyGravity();
			}
		}
	}
	elapsed += Time::deltaTime;

	Game::Update();
}


void AnimatAssignment::Cleanup()
{
	Game::Cleanup();
}

shared_ptr<PhysicsController> AnimatAssignment::createCreature(int size, glm::vec3 position)
{
	glm::vec3 headOrigin = position;
	float headRadius = 1*size;
	float foreheadRadius = headRadius*3;
	int numberOfLegs = 8;
	int legSegments = 3;

	float legSpaceFromHead = 0.5*size;
	float legSegmentSpace = 0*size;
	float bWidth = 0.3*size;
	float bDepth = 0.1*size;
	float bHeight=1.5*size;

	float rotationTheta = 0.0f;
	shared_ptr < PhysicsController> forehead = physicsFactory->CreateSphere(foreheadRadius, headOrigin + glm::vec3(0,foreheadRadius/2, 0), glm::quat());
	forehead->rigidBody->setAngularFactor(btVector3(0, 1, 0));
	shared_ptr<PhysicsController> head = physicsFactory->CreateCylinder(headRadius,headRadius, headOrigin, glm::quat());
	octoHead = head;
	head->rigidBody->setAngularFactor(btVector3(0, 1, 0));
	//octoHeads.push_back(head);
	btPoint2PointConstraint * ballSocket = new btPoint2PointConstraint(*forehead->rigidBody, *head->rigidBody, btVector3(0, -headRadius/2, 0), btVector3(0, foreheadRadius, 0));
	dynamicsWorld->addConstraint(ballSocket);

	shared_ptr<PhysicsController> leg;
	shared_ptr<PhysicsController> lastLeg;

	for (int j = 0; j < numberOfLegs; j++)
	{

		rotationTheta = ((glm::pi<float>() * 2.0f) / numberOfLegs) * j;
		float legX = glm::sin(rotationTheta) * legSpaceFromHead;
		float legZ = -glm::cos(rotationTheta) * legSpaceFromHead;

		float angleOfBoxes = 360 / numberOfLegs;
		float offSetValue = 90.0f;

		glm::vec3 firstSetOfLeg = headOrigin + glm::vec3(legX * 5, (-headRadius-(bHeight/2)), legZ * 5);

		if (j % 2 == 0)
		{
			offSetValue = 0.0f;
		}
		else
		{
			offSetValue = 90.0f;
		}

		leg = physicsFactory->CreateBox(bWidth, bHeight, bDepth, firstSetOfLeg, glm::angleAxis(((angleOfBoxes*j)), glm::vec3(0, 1, 0)));
		if (floating == false)
		{
			testHinge = new btHingeConstraint(*head->rigidBody, *leg->rigidBody, btVector3(legX * 2, -headRadius / 2, legZ * 2), btVector3(0, (bHeight / 2), 0), btVector3(0, 1, 0), btVector3(1, 0, 0), true);
		}
		else
		{
			testHinge = new btHingeConstraint(*head->rigidBody, *leg->rigidBody, btVector3(legX * 2, -headRadius / 2, legZ * 2), btVector3(0, (bHeight / 2), 0), btVector3(0, 0, 0), btVector3(1, 0, 0), true);
		}
		
		testHinge->setLimit(-10, 10);
		octoHinges.push_back(testHinge);
		dynamicsWorld->addConstraint(testHinge);
		lastLeg = leg;

		for (int i = 0; i < legSegments; i++)
		{
			leg = physicsFactory->CreateBox(bWidth, bHeight, bDepth, firstSetOfLeg + glm::vec3(0, (-bHeight - legSegmentSpace)*(i + 1), 0), glm::angleAxis(((angleOfBoxes*j) + offSetValue), glm::vec3(0, 1, 0))
				);
			testHinge = new btHingeConstraint(*lastLeg->rigidBody, *leg->rigidBody, btVector3(0, (-bHeight - legSegmentSpace) / 2, 0), btVector3(0, (bHeight+legSegmentSpace) / 2, 0), btVector3(1, 0, 0), btVector3(1, 0, 0),true);
			testHinge->setLimit(-0, 10);
			octoHinges.push_back(testHinge);
			dynamicsWorld->addConstraint(testHinge);
			lastLeg = leg;
		}
	}
	return head;
}

void AnimatAssignment::generateScene(int numberOfObjects)
{
	for (int i = 0; i < numberOfObjects ; i++)
	{
		physicsFactory->CreateRandomObject(glm::vec3(rand() % -100 + 100, rand() % 50 + 20, rand() % -100 + 100), glm::quat(), glm::vec3(1,1,1));
	}
}