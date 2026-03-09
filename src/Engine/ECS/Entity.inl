#include "Component.h"
#include "Entity.h"

template<typename T>
T* Entity::AddComponent()
{
	T* component = new T();
	component->SetEntity(this);
	m_Components.push_back(component);
	return component;
}

template<typename T>
T* Entity::GetComponent()
{
	for (Component* component : m_Components)
	{
		T* castedComponent = dynamic_cast<T*>(component);
		if (castedComponent != nullptr)
		{
			return castedComponent;
		}
	}
	return nullptr;
}