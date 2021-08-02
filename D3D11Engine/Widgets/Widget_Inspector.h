#pragma once
#include "IWidget.h"

//GUI �� Inspector�� ���õ� Ŭ����
class Widget_Inspector final : public IWidget
{
public:
    //������
	Widget_Inspector(class Context* context);
	//�⺻ �Ҹ���
	~Widget_Inspector() = default;

	//�� ������ ������
	void Render() override;

private:
    //Show Inspector
    void ShowTransform(std::shared_ptr<class Transform>& transform) const;		  //Position, Rotation, Scale ����
	void ShowMaterial(std::shared_ptr<class Material>& material) const;			  //Material Detail ����
	void ShowLight(std::shared_ptr<class Light>& light) const;					  //Light ����
	//void ShowTerrain(std::shared_ptr<class Terrain>& terrain) const;			  //Terrain ���� -> ����� Terrain ����ȭ ���з� ������� ����
	void ShowAnimation(std::shared_ptr<class Animator>& animator);				  //Animation ����
	void ShowScript(std::shared_ptr<class Script>& script) const;				  //Script ����
	void ShowAudioSource(std::shared_ptr<class AudioSource>& audio_source);		  //Audio Source(����� ��ü, �߻���) ����
	void ShowAudioListener(std::shared_ptr<class AudioListener>& audio_listener); //Audio Listener ����

private:
    void ShowAddAnimationButton(); //Animation Component���� 'Add Animation' ��ư�� ������
    void ShowAddComponentButton(); //Inspectorâ���� 'Add Component' ��ư�� ������
	void ShowComponentPopup();     //'Add Component'�� ������ ������ �˾���
	
private:
	//ColorPicker: ImGui���� �������ִ� ���� ���� �˾�
    std::unique_ptr<ColorPicker> material_color_picker;
	std::unique_ptr<ColorPicker> light_color_picker;
};