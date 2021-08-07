#pragma once
#include "stdafx.h"

enum class WidgetType : uint
{
   Console, 
   Hierarchy,
   Inspector,
   MenuBar,  
   ProgressBar,
   Project,
   Scene, 
   ToolBar
};

//GUI의 각각의 요소를 Widget(위젯)이라 부름
//이를 차용하여 모든 GUI에 IWidget 인터페이스 클래스를 상속시키고
//Widget_을 접두사로 네이밍
class IWidget
{
public:
	IWidget(class Context* context) : context(context) {}
	virtual ~IWidget() = default;

	//윈도우창(GUI) 생성
	virtual void Begin()
	{
		if (!is_visible)
			return;

		if (size.x != -1.0f && size.y != -1.0f)
			ImGui::SetNextWindowSize(size);

		if ((size.x != -1.0f && size.y != -1.0f) || (size_max.x != std::numeric_limits<float>::max() && size_max.y != std::numeric_limits<float>::max()))
			ImGui::SetNextWindowSizeConstraints(size, size_max); //윈도우창 크기 제한

	    //윈도우창 생성
		ImGui::Begin(title.c_str(), &is_visible, window_flags);
	}

	//윈도우창(GUI) 매 프레임 렌더링
	virtual void Render() = 0;

	//윈도우창(GUI) 제어 종료
	virtual void End()
	{
		if (!is_visible)
			return;

	    //윈도우창의 폭(높이값)을 반환
		height = ImGui::GetWindowHeight();
		//윈도우창 제어 종료
		ImGui::End();
	}

	auto GetWidgetTitle() const -> const char* { return title.c_str(); }

	auto GetHeight() const -> const float& { return height; }

	auto IsVisible() const -> const bool& { return is_visible; }
	void SetVisible(const bool& is_visible) { this->is_visible = is_visible; }

protected:
	class Context* context = nullptr;

	std::string title = "";

	ImVec2 size = ImVec2(-1.0f, -1.0f);
	ImVec2 size_max = ImVec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());

	float height = 0.0f;
	int window_flags = ImGuiWindowFlags_NoCollapse;

	bool is_visible = true; //생성한 윈도우창 보여주기
};