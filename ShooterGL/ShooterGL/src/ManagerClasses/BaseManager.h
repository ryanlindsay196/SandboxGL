#pragma once
#include "Component.h"
#include <vector>

class BaseManager
{
private:
public:
	BaseManager();
	virtual ~BaseManager();

	template <class t>
	bool AddComponent(std::vector<t>& componentList, unsigned int newEntityComponentIndex, Entity* parentEntity)
	{
		bool componentsAboutToReallocate = (unsigned int)componentList.size() == (unsigned int)(componentList.capacity());
		componentList.push_back(t());

		Component* c = &componentList[componentList.size() - 1];
		c->SetEntityComponentIndex(newEntityComponentIndex);
		c->componentParent = parentEntity;

		if (componentsAboutToReallocate)
		{



			//for (Component component : componentList)
			for (unsigned int i = 0; i < componentList.size(); i++)
			{
#ifdef DEBUG
				t* debugComponent = &componentList[i];
				assert(dynamic_cast<Component*>(debugComponent));
#endif //DEBUG
				t* currentComponent = &componentList[i];
				currentComponent->componentParent->SetComponent(currentComponent->GetEntityComponentIndex(), currentComponent);
				//Changes references to currentComponent in children
				currentComponent->MoveChildReferences();
			}
		}
		//If components list was resized, then this is true, otherwise it is false
		return componentsAboutToReallocate;
	}
};
