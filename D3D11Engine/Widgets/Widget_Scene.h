#pragma once
#include "IWidget.h"

//GUI �� Scene�� ���õ� Ŭ����
class Widget_Scene final : public IWidget
{
public:
	//������
	Widget_Scene(class Context* context);
	//�⺻ �Ҹ���
	~Widget_Scene() = default;

	//�� ������ ������
	//Inspector ���(Scene)�� �� ������ �������� ����� ��
	void Render() override;

private:
   void ShowFrame();		  //�� �����Ӹ��� ����ڿ� ���� ����� Scene ������â�� ũ�⸣ �����ϰ� �������� �ؽ�ó�� ������â�� ���
   void ShowTransformGizmo(); //ImGuizmo���� �������ִ� ����� ���

private:
   class Timer* timer = nullptr;
   class Renderer* renderer = nullptr;
   float frame_counter = 0.0f;
};