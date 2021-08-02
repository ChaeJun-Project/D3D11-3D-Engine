#pragma once
#include "Component/IComponent.h"

//enable_shared_from_this : shared_ptr의 성질을 상속시킴 row->shared로 변환하기 위해 필요
class Actor final : public D3D11_Object , public std::enable_shared_from_this<Actor>
{
public:
	Actor(class Context* context);
	~Actor();

	void Initialize(const std::shared_ptr<class Transform>& transform);

	void Start();
	void Update();
	void Stop();

	//=================================================================================================
	// [Property]
	//=================================================================================================
	auto GetName() const ->const std::string& { return name; }
	auto SetName(const std::string& name) { this->name = name; }

	auto IsActive() const ->const bool& { return is_active; }
	auto SetActive(const bool& is_active) { this->is_active = is_active; }

	auto GetTransform() const -> const std::shared_ptr<class Transform>& { return transform; }
	auto GetTransform_Raw() const -> class Transform* { return transform.get(); }

	auto GetRenderable() const -> const std::shared_ptr<class Renderable>& { return renderable; }
	auto GetRenderable_Raw() const -> class Renderable* { return renderable.get(); }

	//=================================================================================================
	// [Component]
	//=================================================================================================
	auto AddComponent(const ComponentType& type)->std::shared_ptr<IComponent>;

	template <typename T>
	auto AddComponent() -> std::shared_ptr<T>;

	template <typename T>
	auto GetComponent() -> std::shared_ptr<T>;

	template <typename T>
	auto GetComponent_Raw() -> T*;

	auto HasComponent(const ComponentType& type) -> const bool;

	template <typename T>
	auto HasComponent() -> const bool;

	template <typename T>
	void RemoveComponent();

	auto RemoveComponent(const ComponentType& type) -> const bool;

private:
	class Context* context = nullptr;
	std::string name = NOT_ASSIGNED_STR;
	bool is_active = true;

	std::shared_ptr<class Transform> transform;
	std::shared_ptr<class Renderable> renderable;
	std::list<std::shared_ptr<IComponent>> components_list;
};

template<typename T>
inline auto Actor::AddComponent() -> std::shared_ptr<T>
{
	//타입 T가 IComponent를 상속받는 파생클래스일 경우 참 반환
	static_assert(std::is_base_of<IComponent, T>::value, "Provider type does not implement IComponent");

	//타입 T의 타입을 반환
	auto type = IComponent::DeduceComponentType<T>();

	//타입 T가 이미 components_list에 존재하고 Script가 아닌 경우
	if(HasComponent(type) && type != ComponentType::Script)
	  return GetComponent<T>(); //타입 T 반환

	//components_list 맨 뒤에 타입 T 요소 추가
	components_list.push_back
	(
		std::make_shared<T>
		(
		   context,
		   this,
		   transform.get()
		)
	);

	//가장 최근에 추가한 component 참조
	auto new_component = std::dynamic_pointer_cast<T>(components_list.back());

	//component 타입 설정
	new_component->SetComponentType(type);

	//타입 T가 Renderable와 같은 경우
	if constexpr (std::is_same<T, Renderable>::value)
	{
	    //renderable에 해당 component 참조
		renderable = new_component;
	}

	WORK_EVENT(EventType::Scene_Refresh);

	//새로 추가한 컴퍼넌트 반환
	return new_component;
}

template<typename T>
inline auto Actor::GetComponent() -> std::shared_ptr<T>
{
	//타입 T가 IComponent를 상속받는 파생클래스일 경우 참 반환
	static_assert(std::is_base_of<IComponent, T>::value, "Provider type does not implement IComponent");

	//타입 T의 타입을 반환
	auto type = IComponent::DeduceComponentType<T>();

	//components_list를 순회하여 타입 T에 해당하는 component 반환
	for (auto& component : components_list)
	{
		if(component->GetComponentType() == type)
		   return std::dynamic_pointer_cast<T>(component);
	}

	return nullptr;
}

template<typename T>
inline auto Actor::GetComponent_Raw() -> T *
{
	//타입 T가 IComponent를 상속받는 파생클래스일 경우 참 반환
	static_assert(std::is_base_of<IComponent, T>::value, "Provider type does not implement IComponent");

	//타입 T의 타입을 반환
	auto type = IComponent::DeduceComponentType<T>();

	//components_list를 순회하여 타입 T에 해당하는 component의 포인터 반환
	for (auto& component : components_list)
	{
		if (component->GetComponentType() == type)
			return std::dynamic_pointer_cast<T>(component).get();
	}

	return nullptr;
}

template<typename T>
inline auto Actor::HasComponent() -> const bool
{
	//타입 T가 IComponent를 상속받는 파생클래스일 경우 참 반환
	static_assert(std::is_base_of<IComponent, T>::value, "Provider type does not implement IComponent");

	//타입 T의 타입을 반환
	auto type = IComponent::DeduceComponentType<T>();
	
	return HasComponent(type);
}

template<typename T>
inline void Actor::RemoveComponent()
{
	//타입 T가 IComponent를 상속받는 파생클래스일 경우 참 반환
	static_assert(std::is_base_of<IComponent, T>::value, "Provider type does not implement IComponent");

	//타입 T의 타입을 반환
	auto type = IComponent::DeduceComponentType<T>();

	//components_list를 순회하여 타입 T에 해당하는 component 삭제
	for (auto iter = components_list.begin(); iter != components_list.end();)
	{
		auto component = *iter;

		if (component->GetComponentType() == type)
		{
			component.reset();

			iter = components_list.erase(iter); //현재 참조자의 위치에 해당하는 요소를 삭제 후 다음 참조자의 위치를 현재 참조자에게 전달 
		}

		else 
		  iter++;
	}
}
