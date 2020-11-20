#pragma once
#include "D3D11Framework.h"
#include "Subsystem/ISubsystem.h"

//ISubsystem�� �����ϴ� Ŭ����
class Context final
{
public:
	Context() = default;
	~Context()
	{
		//���α׷��� ��Ģ���� ���� �Ҵ��� �����ʹ� ���߿� �����ϴ� ���� �Ϲ����̶�
		//subsystems�� ������ �� ���� ���߿� ���� ������(rbegin)���� ���ʴ�� �޸� ������ ��
		for (auto iter = subsystems.rbegin(); iter != subsystems.rend(); iter++)
			SAFE_DELETE(*iter);
	}

	auto Initialize_Subsystems() -> const bool
	{
		for (const auto& subsystem : subsystems)
		{
			if (!subsystem->Initialize())
			{
				assert(false);
				return false;
			}
		}

		return true;
	}

	template <typename T>
	auto RegisterSubsystem()->T*; //subsystem�� context�� ����ϴ� �Լ�

	template <typename T> 
	auto GetSubsystem()->T*; //context�� ����� subsystem�� ��ȯ�ϴ� �Լ�

private:
	std::vector<ISubsystem*> subsystems;
};

template<typename T>
inline auto Context::RegisterSubsystem() -> T *
{
	//std::is_base_of<class, T>::value : T�� class�� �Ļ�Ŭ����(��, ��Ӱ����� ���)�� ��� value�� true���� ������ �ƴ� ��쿡�� false�� ����
	//static_assert : ���� ������ ���� true�� ��� �׳� �Ѿ���� false�� ��� ������ �޽����� C2338 ������ �����  
	static_assert(std::is_base_of<ISubsystem, T>::value, "Provided type does not implement ISubsystem");
	return static_cast<T*>(subsystems.emplace_back(new T(this)));
}

template<typename T>
inline auto Context::GetSubsystem() -> T *
{
	static_assert(std::is_base_of<ISubsystem, T>::value, "Provided type does not implement ISubsystem");
	for (const auto& subsystem : subsystems)
	{
		if (typeid(T) == typeid(*subsystem))
			return static_cast<T*>(subsystem);
	}
	return nullptr;
}