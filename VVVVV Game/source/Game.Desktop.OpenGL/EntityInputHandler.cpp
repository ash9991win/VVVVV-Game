#include "pch.h"
#include "WorldState.h"
#include "EntityInputHandler.h"

RTTI_DEFINITIONS(EntityInputHandler);

const std::string EntityInputHandler::LEFT_KEY_ATTRIBUTE = "LeftKey";
const std::string EntityInputHandler::RIGHT_KEY_ATTRIBUTE = "RightKey";
const std::string EntityInputHandler::SWITCH_GRAVITY_KEY_ATTRIBUTE = "SwitchGravityKey";
const std::string EntityInputHandler::WORLD_STATE_ATTRIBUTE = "WorldState";

const std::uint32_t defaultHashCapacity = 15U;

EntityInputHandler::EntityInputHandler() : mLeftKey(GLFW_KEY_LEFT), mRightKey(GLFW_KEY_RIGHT), mSwitchGravityKey(GLFW_KEY_SPACE), mKeyHandlers(defaultHashCapacity), mWorldState(nullptr)
{
	AddExternalAttribute(LEFT_KEY_ATTRIBUTE, 1, &mLeftKey);
	AddExternalAttribute(RIGHT_KEY_ATTRIBUTE, 1, &mRightKey);
	AddExternalAttribute(SWITCH_GRAVITY_KEY_ATTRIBUTE, 1, &mSwitchGravityKey);
	AddExternalAttribute(WORLD_STATE_ATTRIBUTE, 1, reinterpret_cast<RTTI**>(&mWorldState));
}

int EntityInputHandler::LeftKey() const
{
	return mLeftKey;
}

void EntityInputHandler::SetLeftKey(int key)
{
	mLeftKey = key;
}

int EntityInputHandler::RightKey() const
{
	return mRightKey;
}

void EntityInputHandler::SetRightKey(int key)
{
	mRightKey = key;
}

int EntityInputHandler::SwitchGravityKey() const
{
	return mSwitchGravityKey;
}

void EntityInputHandler::SetSwitchGravityKey(int key)
{
	mSwitchGravityKey = key;
}

const WorldState & EntityInputHandler::GetWorldState() const
{
	if (mWorldState == nullptr)
	{
		throw std::exception("Does not have a world state!");
	}

	return *mWorldState;
}

void EntityInputHandler::SetWorldState(const WorldState & world)
{
	mWorldState = &const_cast<WorldState&>(world);
}

void EntityInputHandler::GenerateHandlers()
{
	mKeyHandlers.Clear();

	mKeyHandlers.Insert(std::make_pair(mLeftKey, &EntityInputHandler::HandleLeftKey));
	mKeyHandlers.Insert(std::make_pair(mRightKey, &EntityInputHandler::HandleRightKey));
	mKeyHandlers.Insert(std::make_pair(mSwitchGravityKey, &EntityInputHandler::HandleSwitchGravityKey));
}

void EntityInputHandler::HandleKey(int key, int action)
{
	if (mKeyHandlers.ContainsKey(key))
	{
		(this->*mKeyHandlers.Find(key)->second)(action);
	}
}

void EntityInputHandler::HandleLeftKey(int action)
{
	UNREFERENCED_PARAMETER(action);

	if (action == GLFW_PRESS)
	{
		SendInputMessage("Left");
	}
	else if (action == GLFW_RELEASE)
	{
		SendInputMessage("LeftStop");
	}
	
}

void EntityInputHandler::HandleRightKey(int action)
{
	if (action == GLFW_PRESS)
	{
		SendInputMessage("Right");
	}
	else if (action == GLFW_RELEASE)
	{
		SendInputMessage("RightStop");
	}

	
}

void EntityInputHandler::HandleSwitchGravityKey(int action)
{
	if (action == GLFW_PRESS)
	{
		SendInputMessage("Flip");
	}
	else if (action == GLFW_RELEASE)
	{

	}
	
}

void EntityInputHandler::SendInputMessage(std::string subtype)
{
	try
	{
		if (!mWorldState->GetUnloadSector())
		{
			EventMessageAttributed ema;
			ema.SetSubtype(subtype);
			ema.SetWorld(*mWorldState->mWorld);

			std::shared_ptr<Event<EventMessageAttributed>> eventMessage(new Event<EventMessageAttributed>(ema));
			GetWorldState().mWorld->GetEventQueue().Enqueue(eventMessage, mWorldState->GetGameTime());
		}
	}
	catch(const std::exception& e)
	{
		std::cout << e.what();
	}

}

