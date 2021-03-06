/* 
 * esmini - Environment Simulator Minimalistic 
 * https://github.com/esmini/esmini
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * Copyright (c) partners of Simulation Scenarios
 * https://sites.google.com/view/simulationscenarios
 */

#include "Story.hpp"
#include "CommonMini.hpp"

using namespace scenarioengine;


Story::Story()
{
	LOG("Story: New Story created");
}

Act* Story::FindActByName(std::string name)
{
	for (size_t i = 0; i < act_.size(); i++)
	{
		if (name == act_[i]->name_)
		{
			return act_[i];
		}
	}

	return nullptr;
}

Event* Story::FindEventByName(std::string name)
{
	for (size_t i = 0; i < act_.size(); i++)
	{
		for (size_t j = 0; j < act_[i]->sequence_.size(); j++)
		{
			for (size_t k = 0; k < act_[i]->sequence_[j]->maneuver_.size(); k++)
			{
				for (size_t l = 0; l < act_[i]->sequence_[j]->maneuver_[k]->event_.size(); l++)
				{
					Event *event = act_[i]->sequence_[j]->maneuver_[k]->event_[l];
					if (name == event->name_)
					{
						return event;
					}
				}
			}
		}
	}

	return nullptr;
}

OSCAction * Story::FindActionByName(std::string name)
{
	for (size_t i = 0; i < act_.size(); i++)
	{
		for (size_t j = 0; j < act_[i]->sequence_.size(); j++)
		{
			for (size_t k = 0; k < act_[i]->sequence_[j]->maneuver_.size(); k++)
			{
				for (size_t l = 0; l < act_[i]->sequence_[j]->maneuver_[k]->event_.size(); l++)
				{
					for (size_t m = 0; m < act_[i]->sequence_[j]->maneuver_[k]->event_[l]->action_.size(); m++)
					{
						OSCAction *action = act_[i]->sequence_[j]->maneuver_[k]->event_[l]->action_[m];
						if (name == action->name_)
						{
							return action;
						}
					}
				}
			}
		}
	}

	return nullptr;
}

void Story::Print()
{
	LOG("Story: %s", name_.c_str());
}

void Story::Step(double dt)
{
	(void)dt;
//	for(int i=0; i<Act)
}

