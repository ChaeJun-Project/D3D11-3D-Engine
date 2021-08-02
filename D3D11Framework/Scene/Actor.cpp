#include "D3D11Framework.h"
#include "Actor.h"
#include "Scene.h"
#include "Component/Transform.h"
#include "Component/Renderable.h"
#include "Component/Camera.h"
#include "Component/Light.h"
#include "Component/Terrain.h"
#include "Component/SkyBox.h"
#include "Component/Script.h"
#include "Component/AudioSource.h"
#include "Component/AudioListener.h"
#include "Component/Animator.h"

Actor::Actor(Context * context)
	:context(context)
{

}

Actor::~Actor()
{
	//components_list를 순회하여 리스트 내의 shared_ptr 모두 메모리 해제
	for (auto& component : components_list)
	{
		component.reset();
	}

	//components_list 요소 전부 삭제
	components_list.clear();
}

void Actor::Initialize(const std::shared_ptr<class Transform>& transform)
{
	this->transform = transform;
}

void Actor::Start()
{
	if (!is_active)
		return;

	//components_list를 순회하여
	for (auto& component : components_list)
	{
		if (!component->IsEnabled())
		{
			continue;
		}

		component->OnStart();
	}
}

void Actor::Update()
{
	if (!is_active)
		return;

	//components_list를 순회하여
	for (auto& component : components_list)
	{
		if (!component->IsEnabled())
		{
			continue;
		}

		component->OnUpdate();
	}
}

void Actor::Stop()
{
	if (!is_active)
		return;

	//components_list를 순회하여
	for (auto& component : components_list)
	{
		if (!component->IsEnabled())
		{
			continue;
		}

		component->OnStop();
	}
}

auto Actor::AddComponent(const ComponentType & type) -> std::shared_ptr<IComponent>
{
	std::shared_ptr<IComponent> component;

	switch (type)
	{
	case ComponentType::Transform:      component = AddComponent<Transform>();      break;
	case ComponentType::Renderable:     component = AddComponent<Renderable>();     break;
	case ComponentType::Camera:         component = AddComponent<Camera>();         break;
	case ComponentType::Light:          component = AddComponent<Light>();          break;
	case ComponentType::Terrain:        component = AddComponent<Terrain>();        break;
	case ComponentType::SkyBox:         component = AddComponent<SkyBox>();         break;
	case ComponentType::Script:         component = AddComponent<Script>();         break;
	case ComponentType::AudioSource:    component = AddComponent<AudioSource>();    break;
	case ComponentType::AudioListener:  component = AddComponent<AudioListener>();  break;
	case ComponentType::Animator:      component = AddComponent<Animator>();      break;
	}

	return component;
}

auto Actor::HasComponent(const ComponentType & type) -> const bool
{
	for (const auto& component : components_list)
	{
		if (component->GetComponentType() == type)
			return true;
	}
	return false;
}

auto Actor::RemoveComponent(const ComponentType & type) -> const bool
{
	//components_list를 순회하여 타입 T에 해당하는 component 삭제
	for (auto iter = components_list.begin(); iter != components_list.end();)
	{
		auto component = *iter;

		if (component->GetComponentType() == type)
		{
			component.reset();

			iter = components_list.erase(iter); //현재 참조자의 위치에 해당하는 요소를 삭제 후 다음 참조자의 위치를 현재 참조자에게 전달 

			return true;
		}

		else
			iter++;
	}

	//해당 type의 component가 없는 경우
	return false;
}
