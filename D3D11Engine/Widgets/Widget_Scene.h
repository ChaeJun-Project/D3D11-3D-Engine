#pragma once
#include "IWidget.h"

//GUI 중 Scene과 관련된 클래스
class Widget_Scene final : public IWidget
{
public:
	//생성자
	Widget_Scene(class Context* context);
	//기본 소멸자
	~Widget_Scene() = default;

	//매 프레임 렌더링
	//Inspector 요소(Scene)는 매 프레임 렌더링을 해줘야 함
	void Render() override;

private:
   void ShowFrame();		  //매 프레임마다 사용자에 의해 변경된 Scene 윈도우창의 크기르 설정하고 렌더링된 텍스처를 윈도우창에 등록
   void ShowTransformGizmo(); //ImGuizmo에서 제공해주는 기즈모 사용

private:
   class Timer* timer = nullptr;
   class Renderer* renderer = nullptr;
   float frame_counter = 0.0f;
};