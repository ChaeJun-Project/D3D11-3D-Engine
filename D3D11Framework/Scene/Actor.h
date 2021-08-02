#pragma once
#include "Component/IComponent.h"

//enable_shared_from_this : shared_ptr�� ������ ��ӽ�Ŵ row->shared�� ��ȯ�ϱ� ���� �ʿ�
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
	//Ÿ�� T�� IComponent�� ��ӹ޴� �Ļ�Ŭ������ ��� �� ��ȯ
	static_assert(std::is_base_of<IComponent, T>::value, "Provider type does not implement IComponent");

	//Ÿ�� T�� Ÿ���� ��ȯ
	auto type = IComponent::DeduceComponentType<T>();

	//Ÿ�� T�� �̹� components_list�� �����ϰ� Script�� �ƴ� ���
	if(HasComponent(type) && type != ComponentType::Script)
	  return GetComponent<T>(); //Ÿ�� T ��ȯ

	//components_list �� �ڿ� Ÿ�� T ��� �߰�
	components_list.push_back
	(
		std::make_shared<T>
		(
		   context,
		   this,
		   transform.get()
		)
	);

	//���� �ֱٿ� �߰��� component ����
	auto new_component = std::dynamic_pointer_cast<T>(components_list.back());

	//component Ÿ�� ����
	new_component->SetComponentType(type);

	//Ÿ�� T�� Renderable�� ���� ���
	if constexpr (std::is_same<T, Renderable>::value)
	{
	    //renderable�� �ش� component ����
		renderable = new_component;
	}

	WORK_EVENT(EventType::Scene_Refresh);

	//���� �߰��� ���۳�Ʈ ��ȯ
	return new_component;
}

template<typename T>
inline auto Actor::GetComponent() -> std::shared_ptr<T>
{
	//Ÿ�� T�� IComponent�� ��ӹ޴� �Ļ�Ŭ������ ��� �� ��ȯ
	static_assert(std::is_base_of<IComponent, T>::value, "Provider type does not implement IComponent");

	//Ÿ�� T�� Ÿ���� ��ȯ
	auto type = IComponent::DeduceComponentType<T>();

	//components_list�� ��ȸ�Ͽ� Ÿ�� T�� �ش��ϴ� component ��ȯ
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
	//Ÿ�� T�� IComponent�� ��ӹ޴� �Ļ�Ŭ������ ��� �� ��ȯ
	static_assert(std::is_base_of<IComponent, T>::value, "Provider type does not implement IComponent");

	//Ÿ�� T�� Ÿ���� ��ȯ
	auto type = IComponent::DeduceComponentType<T>();

	//components_list�� ��ȸ�Ͽ� Ÿ�� T�� �ش��ϴ� component�� ������ ��ȯ
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
	//Ÿ�� T�� IComponent�� ��ӹ޴� �Ļ�Ŭ������ ��� �� ��ȯ
	static_assert(std::is_base_of<IComponent, T>::value, "Provider type does not implement IComponent");

	//Ÿ�� T�� Ÿ���� ��ȯ
	auto type = IComponent::DeduceComponentType<T>();
	
	return HasComponent(type);
}

template<typename T>
inline void Actor::RemoveComponent()
{
	//Ÿ�� T�� IComponent�� ��ӹ޴� �Ļ�Ŭ������ ��� �� ��ȯ
	static_assert(std::is_base_of<IComponent, T>::value, "Provider type does not implement IComponent");

	//Ÿ�� T�� Ÿ���� ��ȯ
	auto type = IComponent::DeduceComponentType<T>();

	//components_list�� ��ȸ�Ͽ� Ÿ�� T�� �ش��ϴ� component ����
	for (auto iter = components_list.begin(); iter != components_list.end();)
	{
		auto component = *iter;

		if (component->GetComponentType() == type)
		{
			component.reset();

			iter = components_list.erase(iter); //���� �������� ��ġ�� �ش��ϴ� ��Ҹ� ���� �� ���� �������� ��ġ�� ���� �����ڿ��� ���� 
		}

		else 
		  iter++;
	}
}
