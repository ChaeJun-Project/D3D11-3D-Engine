#pragma once

#include "Widgets/IWidget.h"

//D3D11 Engine의 전반적인 GUI 초기세팅 및 엔진 업데이트 및 렌더링 로직과 관련된 클래스
class Editor final
{
public:
    //생성자
    Editor();
	//소멸자
    ~Editor();

	//윈도우창 사이즈 조정
    void Resize(const uint& width, const uint& height);
	//매 프레임 업데이트
    void Update();
	//매 프레임 렌더링
    void Render();

private:
    //ImGui Docking
    void BeginDockspace();
    void EndDockspace();

	//Set GUI Style
    void ApplyStyle();

private:
    //D3D11 Framework의 로직을 관리하는 클래스
	//하나의 D3D11 Engine 프로세스에서 유일한 단 하나의 Engine 클래스만 가지고 있어야 하므로 
	//unique pointer를 사용하여 객체 생성 및 관리
    std::unique_ptr<class Engine> engine;

    class Context* context      = nullptr;
    class Graphics* graphics    = nullptr;

	//엔전에서 사용되는 모든 GUI를 저장 및 관리할 벡터
	//각 GUI 또한 각 종류마다 유일한 단 하나의 객체만 가지고 있어야 하므로
	//unique pointer를 사용하여 객체 생성 및 관리
	std::map<WidgetType, std::unique_ptr<class IWidget>> widgets_map;

	//ImGui Docking System을 사용하기 위해 사용할 플래그 변수
    bool is_using_dock_space    = true;
};
