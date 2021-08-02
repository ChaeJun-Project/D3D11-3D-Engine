#pragma once
#include "stdafx.h"

//Payload(페이로드): 보내고자 하는 데이터 자체를 의미

//페이로드 타입 종류
enum class PayloadType : uint
{
	Unknown, 
	Texture,	//텍스처 데이터
	Model,		//3D Model 데이터
	Audio,		//오디오 데이터
	Script		//스크립트 데이터
};

//드래그&드롭으로 전송할 데이터 구조체
struct DragDropPayload final
{
	DragDropPayload(const PayloadType& type = PayloadType::Unknown, const std::variant<const char*, const std::string>& data = nullptr)
		:type(type)
		, data(data)
	{

	}

	PayloadType type; //페이로드 타입
	//std::variant<T,C, ...>: 컴파일 타임에 정해진 여러가지 타입들 중에 한 가지 타입의 객체를 보관할 수 있는 클래스
	//여기서는 문자열 상수와 스트링 상수만을 데이터 타입으로 가질 수 있도록 설정
	std::variant<const char*, const std::string> data; //페이로드 데이터 자체
};

//참고: https://github.com/ocornut/imgui/issues/1931

//reinterpret_cast<T>()
//int -> pointer, pointer -> int, pointer1->pointer2(pointer1과 2는 서로 아무런 관계가 없음) 등 
//변환을 가능하게 해주는 캐스트 연산자. 하지만 해당 주소 값이 유효한지 따로 검사하지 않고 실행 시 런타임 오류가 날 수 있으므로 신중하게 사용
class DragDropEvent final
{
public:
	//DragDropPayload 생성 -> Imgui에 Drag&Drop Payload를 등록
	static void CreateDragDropPayload(const DragDropPayload& payload)
	{
	    //Imgui에 Drag&Drop Payload를 등록
		ImGui::SetDragDropPayload
		(
			reinterpret_cast<const char*>(&payload.type), //페이로드 타입 포인터
			reinterpret_cast<const void*>(&payload), //페이로드 포인터
			sizeof(payload), //페이로드 데이터 사이즈
			ImGuiCond_Once 
		);
	}

	//해당 PayloadType 타입의 DragDropPayload 받기
	static  DragDropPayload* ReceiveDragDropPayload(const PayloadType& type)
	{
	    //Imgui 내에서 DragDrop을 판별
		if (ImGui::BeginDragDropTarget())
		{
		   if(const auto payload = ImGui::AcceptDragDropPayload(reinterpret_cast<const char*>(&type)))
		      return static_cast<DragDropPayload*>(payload->Data); //해당 PayloadType 타입의 페이로드가 존재하면 반환

		   ImGui::EndDragDropTarget();
	    }

		return nullptr;
	}
};