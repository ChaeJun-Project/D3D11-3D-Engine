#pragma once

#include "Widgets/IWidget.h"

//D3D11 Engine�� �������� GUI �ʱ⼼�� �� ���� ������Ʈ �� ������ ������ ���õ� Ŭ����
class Editor final
{
public:
    //������
    Editor();
	//�Ҹ���
    ~Editor();

	//������â ������ ����
    void Resize(const uint& width, const uint& height);
	//�� ������ ������Ʈ
    void Update();
	//�� ������ ������
    void Render();

private:
    //ImGui Docking
    void BeginDockspace();
    void EndDockspace();

	//Set GUI Style
    void ApplyStyle();

private:
    //D3D11 Framework�� ������ �����ϴ� Ŭ����
	//�ϳ��� D3D11 Engine ���μ������� ������ �� �ϳ��� Engine Ŭ������ ������ �־�� �ϹǷ� 
	//unique pointer�� ����Ͽ� ��ü ���� �� ����
    std::unique_ptr<class Engine> engine;

    class Context* context      = nullptr;
    class Graphics* graphics    = nullptr;

	//�������� ���Ǵ� ��� GUI�� ���� �� ������ ����
	//�� GUI ���� �� �������� ������ �� �ϳ��� ��ü�� ������ �־�� �ϹǷ�
	//unique pointer�� ����Ͽ� ��ü ���� �� ����
	std::map<WidgetType, std::unique_ptr<class IWidget>> widgets_map;

	//ImGui Docking System�� ����ϱ� ���� ����� �÷��� ����
    bool is_using_dock_space    = true;
};
