#pragma once
#include "IWidget.h"

//GUI 중 Inspector와 관련된 클래스
class Widget_Inspector final : public IWidget
{
public:
    //생성자
	Widget_Inspector(class Context* context);
	//기본 소멸자
	~Widget_Inspector() = default;

	//매 프레임 렌더링
	void Render() override;

private:
    //Show Inspector
    void ShowTransform(std::shared_ptr<class Transform>& transform) const;		  //Position, Rotation, Scale 관련
	void ShowMaterial(std::shared_ptr<class Material>& material) const;			  //Material Detail 관련
	void ShowLight(std::shared_ptr<class Light>& light) const;					  //Light 관련
	//void ShowTerrain(std::shared_ptr<class Terrain>& terrain) const;			  //Terrain 관련 -> 현재는 Terrain 최적화 실패로 사용하지 않음
	void ShowAnimation(std::shared_ptr<class Animator>& animator);				  //Animation 관련
	void ShowScript(std::shared_ptr<class Script>& script) const;				  //Script 관련
	void ShowAudioSource(std::shared_ptr<class AudioSource>& audio_source);		  //Audio Source(오디오 주체, 발생지) 관련
	void ShowAudioListener(std::shared_ptr<class AudioListener>& audio_listener); //Audio Listener 관련

private:
    void ShowAddAnimationButton(); //Animation Component에서 'Add Animation' 버튼을 보여줌
    void ShowAddComponentButton(); //Inspector창에서 'Add Component' 버튼을 보여줌
	void ShowComponentPopup();     //'Add Component'을 누르면 보여줄 팝업들
	
private:
	//ColorPicker: ImGui에서 지원해주는 색상 선택 팝업
    std::unique_ptr<ColorPicker> material_color_picker;
	std::unique_ptr<ColorPicker> light_color_picker;
};