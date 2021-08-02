#pragma once
#include "stdafx.h"

//Payload(���̷ε�): �������� �ϴ� ������ ��ü�� �ǹ�

//���̷ε� Ÿ�� ����
enum class PayloadType : uint
{
	Unknown, 
	Texture,	//�ؽ�ó ������
	Model,		//3D Model ������
	Audio,		//����� ������
	Script		//��ũ��Ʈ ������
};

//�巡��&������� ������ ������ ����ü
struct DragDropPayload final
{
	DragDropPayload(const PayloadType& type = PayloadType::Unknown, const std::variant<const char*, const std::string>& data = nullptr)
		:type(type)
		, data(data)
	{

	}

	PayloadType type; //���̷ε� Ÿ��
	//std::variant<T,C, ...>: ������ Ÿ�ӿ� ������ �������� Ÿ�Ե� �߿� �� ���� Ÿ���� ��ü�� ������ �� �ִ� Ŭ����
	//���⼭�� ���ڿ� ����� ��Ʈ�� ������� ������ Ÿ������ ���� �� �ֵ��� ����
	std::variant<const char*, const std::string> data; //���̷ε� ������ ��ü
};

//����: https://github.com/ocornut/imgui/issues/1931

//reinterpret_cast<T>()
//int -> pointer, pointer -> int, pointer1->pointer2(pointer1�� 2�� ���� �ƹ��� ���谡 ����) �� 
//��ȯ�� �����ϰ� ���ִ� ĳ��Ʈ ������. ������ �ش� �ּ� ���� ��ȿ���� ���� �˻����� �ʰ� ���� �� ��Ÿ�� ������ �� �� �����Ƿ� �����ϰ� ���
class DragDropEvent final
{
public:
	//DragDropPayload ���� -> Imgui�� Drag&Drop Payload�� ���
	static void CreateDragDropPayload(const DragDropPayload& payload)
	{
	    //Imgui�� Drag&Drop Payload�� ���
		ImGui::SetDragDropPayload
		(
			reinterpret_cast<const char*>(&payload.type), //���̷ε� Ÿ�� ������
			reinterpret_cast<const void*>(&payload), //���̷ε� ������
			sizeof(payload), //���̷ε� ������ ������
			ImGuiCond_Once 
		);
	}

	//�ش� PayloadType Ÿ���� DragDropPayload �ޱ�
	static  DragDropPayload* ReceiveDragDropPayload(const PayloadType& type)
	{
	    //Imgui ������ DragDrop�� �Ǻ�
		if (ImGui::BeginDragDropTarget())
		{
		   if(const auto payload = ImGui::AcceptDragDropPayload(reinterpret_cast<const char*>(&type)))
		      return static_cast<DragDropPayload*>(payload->Data); //�ش� PayloadType Ÿ���� ���̷ε尡 �����ϸ� ��ȯ

		   ImGui::EndDragDropTarget();
	    }

		return nullptr;
	}
};