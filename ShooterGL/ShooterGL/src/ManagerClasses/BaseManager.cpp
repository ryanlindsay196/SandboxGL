#include "BaseManager.h"



BaseManager::BaseManager()
{
}


BaseManager::~BaseManager()
{
}


//template<class t>
//void BaseManager::AddComponent(std::vector<t>& componentList)
//{
//	bool componentsAboutToReallocate = (unsigned int)componentList.size() == (unsigned int)(componentList.capacity - 1);
//	componentList.push_back(t());
//	if (componentsAboutToReallocate)
//	{
//		for (unsigned int i = 0; i < componentList.size(); i++)
//		{
//			componentList[i].componentParent->SetComponent(componentList[i].EntityComponentIndex(), componentList[i]);
//		}
//	}
//}