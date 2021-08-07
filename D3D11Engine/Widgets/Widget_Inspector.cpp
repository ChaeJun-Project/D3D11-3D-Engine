#include "stdafx.h"
#include "Widget_Inspector.h"
#include "Scene/Actor.h"
#include "Scene/Component/Camera.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/Light.h"
#include "Scene/Component/Renderable.h"
#include "Scene/Component/Terrain.h"
#include "Scene/Component/Script.h"
#include "Scene/Component/AudioSource.h"
#include "Scene/Component/AudioListener.h"
#include "Scene/Component/Animator.h"

//추가한 컴퍼넌트를 보여줄 박스와 특정 컴퍼넌트를 삭제할 수 있도록 기능 제공
namespace Inspector_Data
{
	static std::string popup_name;

	//특정 컴퍼넌트 옵션 팝업
	inline void ComponentMenuPopup(const std::string& popup_id, const ComponentType& component_type, const std::shared_ptr<IComponent>& component, const bool& is_removeable)
	{
		if (ImGui::BeginPopup(popup_id.c_str()))
		{
			if (is_removeable)
			{
				//제거 버튼을 눌렀을 경우
				if (ImGui::MenuItem("Remove"))
				{
					if (auto actor = Editor_Helper::GetInstance()->selected_actor.lock())
					{
						if (actor->RemoveComponent(component_type))
						{
							LOG_INFO_F("Remove %s Component in '%s' Object", popup_id.c_str(), actor->GetName().c_str());
						}

						else
						{
							LOG_ERROR_F("Fail to Remove %s Component in '%s' Object", popup_id.c_str(), actor->GetName().c_str());
						}

					}
				}
			}

			ImGui::EndPopup();
		}
	}

	//특정 컴퍼넌트 요소 박스 만들기 
	inline const bool ComponentBegin(const std::string& name, const IconType& type, const ComponentType& component_type, const std::shared_ptr<IComponent>& component, const bool& is_option = true, const bool& is_removeable = true)
	{
		//컴퍼넌트 요소 박스의 헤더 만들기
		const auto collapsed_header = ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen);

		ImGui::SameLine(); //컴퍼넌트 헤더와 같은 라인
		ImGui::Spacing(); //컴퍼넌트 타이틀 옆에 빈 공간 만들기 
		ImGui::SameLine(); //컴퍼넌트 헤더와 같은 라인

		const auto original_pen_y = ImGui::GetCursorPosY();

		ImGui::SetCursorPosY(original_pen_y + 4.0f); //현재 UI를 그릴 y축보다 밑으로 4만큼 이동

		//컴퍼넌트 아이콘 만들기
		Icon_Provider::Get().Image(type, 15.0f);

		if (is_option)
		{
			ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 15.0f);
			ImGui::SetCursorPosY(original_pen_y);

			//옵션 아이콘&버튼 만들기
			if (Icon_Provider::Get().ImageButton(name.c_str(), IconType::Button_Option, 12.0f))
			{
				Inspector_Data::popup_name = name;
				//컴퍼넌트의 이름으로 컴퍼넌트 관리 팝업 만들기
				ImGui::OpenPopup(Inspector_Data::popup_name.c_str());
			}

			//현재 컴퍼넌트 옵션 버튼을 누른 컴퍼넌트와 현재 저장된 팝업의 이름과 같은 경우
			//컴퍼넌트 관리 팝업 띄워주기
			if (Inspector_Data::popup_name == name)
				//Component 관리 메뉴 생성
				ComponentMenuPopup(Inspector_Data::popup_name, component_type, component, is_removeable);
		}

		return collapsed_header;
	}

	//컴퍼넌트를 구분할 구분 선 그리기
	inline void ComponentEnd()
	{
		ImGui::Separator();
	}
}

Widget_Inspector::Widget_Inspector(Context * context)
	:IWidget(context)
{
	//해당 GUI 타이틀 초기화
	title = "Inspector";

	//ImGui에서 지원해주는 ColorPicker 사용(Material 전용)
	material_color_picker = std::make_unique<ColorPicker>("Material Color Picker");

	//ImGui에서 지원해주는 ColorPicker 사용(Light 전용)
	light_color_picker = std::make_unique<ColorPicker>("Light Color Picker");
}

void Widget_Inspector::Render()
{
	//각 컴퍼넌트 창의 너비를 100으로 고정
	ImGui::PushItemWidth(max_width);
	{
		if (auto actor = Editor_Helper::GetInstance()->selected_actor.lock())
		{
			//선택된 Actor에 Transform Component가 포함된 경우
			if (auto transform = actor->GetComponent<Transform>())
				ShowTransform(transform); //Transform Component 보여주기

			//선택된 Actor에 Material Component가 포함된 경우
			if (auto renderable = actor->GetComponent<Renderable>())
			{
				if (auto material = renderable->GetMaterial())
					ShowMaterial(material); //Material Component 보여주기
			}

			//선택된 Actor에 Light Component가 포함된 경우
			if (auto light = actor->GetComponent<Light>())
				ShowLight(light); //Light Component 보여주기

			/*if (auto terrain = actor->GetComponent<Terrain>())
			ShowTerrain(terrain);*/

			//선택된 Actor에 Animator Component가 포함된 경우
			if (auto animator = actor->GetComponent<Animator>())
				ShowAnimation(animator); //Animaton Component 보여주기

			//선택된 Actor에 Script Component가 포함된 경우
			if (auto script = actor->GetComponent<Script>())
				ShowScript(script); //Script Component 보여주기

			//선택된 Actor에 AudioSource Component가 포함된 경우
			if (auto audio_source = actor->GetComponent<AudioSource>())
				ShowAudioSource(audio_source); //AudioSource Component 보여주기

			//선택된 Actor에 AudioListener Component가 포함된 경우
			if (auto audio_listener = actor->GetComponent<AudioListener>())
				ShowAudioListener(audio_listener); //AudioListener Component 보여주기
		}

		//Add Component Button 렌더링
		ShowAddComponentButton();
	}
	//각 컴퍼넌트 창의 너비를 100으로 고정 풀기
	ImGui::PopItemWidth();
}

void Widget_Inspector::ShowTransform(std::shared_ptr<class Transform>& transform) const
{
	if (!transform.get())
		return;

	//Transform 컴퍼넌트 요소 박스 만들기 
	if (Inspector_Data::ComponentBegin("Transform", IconType::Component_Transform, ComponentType::Transform, transform, false, false))
	{
		auto position = transform->GetTranslation();
		auto rotation = transform->GetRotation().ToEulerAngle();
		auto scale = transform->GetScale();

		//람다 함수 show_float
		//람다를 담을 변수는 상수화 되어야 하므로 const를 
		const auto show_float = [](const char* label, float* value)
		{
			ImGui::TextUnformatted(label);
			ImGui::SameLine();

			ImGui::PushItemWidth(60.0f);
			ImGui::PushID(static_cast<int>(ImGui::GetCursorPosX() + ImGui::GetCursorPosY())); //위치에 따라 ID를 설정
			 //해당 Input Box를 드래그하여 
			 //value값을 float 타입의 최소,최대값까지 증가할 수 있도록 설정
			ImGui::DragFloat("##no_label", value, 0.1f, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());
			ImGui::PopID();
			ImGui::PopItemWidth();
		};

		//람다 함수 show_vector3
		//show_float 람다 함수 캡처
		const auto show_vector3 = [&show_float](const char* label, Vector3& value)
		{
			ImGui::BeginGroup(); //하나의 그룹으로 묶기
			ImGui::Indent(15.0f); //들여쓰기
			ImGui::TextUnformatted(label); //라벨(Position, Rotation, Scale)
			show_float("X", &value.x); ImGui::SameLine(100.0f); //같은 라인에 시작부터 100만큼 떨어진 곳부터
			show_float("Y", &value.y); ImGui::SameLine(185.0f); //같은 라인에 시작부터 185만큼 떨어진 곳부터
			show_float("Z", &value.z);
			ImGui::EndGroup(); //하나의 그룹으로 묶기 해제
		};

		show_vector3("Position", position); //Position
		show_vector3("Rotation", rotation); //Rotation
		show_vector3("Scale", scale);		//Scale

		//게임모드가 아닐 경우 편집 가능
		if (!Engine::IsFlagEnabled(EngineFlags_Game))
		{
			transform->SetTranslation(position); //Position 재설정
			transform->SetRotation(Quaternion::QuaternionFromEulerAngle(rotation)); //Rotation 재설정
			transform->SetScale(scale); //Scale 재설정
		}
	}
	//컴퍼넌트를 구분할 구분 선 그리기
	Inspector_Data::ComponentEnd();
}

void Widget_Inspector::ShowMaterial(std::shared_ptr<class Material>& material) const
{
	if (!material.get())
		return;

	//Material 컴퍼넌트 요소 박스 만들기 
	if (Inspector_Data::ComponentBegin("Material", IconType::Component_Material, ComponentType::Unknown, nullptr, false, false))
	{
		auto roughness = material->GetRoughnessCoefficient(); //거친정도(수치)
		auto metallic = material->GetMetallicCoefficient();   //금속정도(수치)
		auto normal = material->GetNormalCoefficient();		  //입체정도(수치)
		auto height = material->GetHeightCoefficient();		  //높이정도(수치)
		auto uv_offset = material->GetOffset();				  //텍스처 오프셋(시작점)(수치)
		auto uv_tiling = material->GetTiling();				  //텍스처 타일링(수치)

		material_color_picker->SetColor(material->GetAlbedoColor()); //ImGui에서 지원해주는 ColorPicker 사용(Material 전용)

		const auto albedo_texture = material->GetTexture(TextureType::Albedo);			//입체감 없는 단순한 색
		const auto roughness_texture = material->GetTexture(TextureType::Roughness);	//표면의 거친정도
		const auto metallic_texture = material->GetTexture(TextureType::Metallic);		//표면이 얼마나 금속같은 지
		const auto normal_texture = material->GetTexture(TextureType::Normal);			//표면의 입체감을 표현
		const auto height_texture = material->GetTexture(TextureType::Height);			//표면의 y값을 이미지 파일로 저장한 것

		//람다 함수 show_texture_slot
		//오브젝트에 적용한 텍스처를 Inspector창에서 이미지로 보여주는 함수
		const auto show_texture_slot = [&material](const char* name, const std::shared_ptr<ITexture>& texture, const TextureType& type)
		{
			ImGui::TextUnformatted(name);
			ImGui::SameLine(70.0f);
			ImGui::Image
			(
				texture ? texture->GetShaderResourceView() : nullptr,
				ImVec2(80.0f, 80.0f),
				ImVec2(0.0f, 0.0f),
				ImVec2(1.0f, 1.0f),
				ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
				ImColor(1.0f, 1.0f, 1.0f, 0.5f)
			);

			//Drag&Drop Event
			//Payload Data(Texture)
			if (auto payload = DragDropEvent::ReceiveDragDropPayload(PayloadType::Texture))
			{
			    //Load Payload(Texture) 
				if (const auto resource = Editor_Helper::GetInstance()->resource_manager->Load<Texture2D>(std::get<const char*>(payload->data)))
					material->SetTexture(type, resource);
			}

			//Remove Button
			if (material->HasTexture(type))
			{
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 30.0f);
				if (Icon_Provider::Get().ImageButton(name, IconType::Button_Remove, 15.0f)) //제거 버튼을 눌렀을 때
					material->SetTexture(type, std::shared_ptr<ITexture>()); //빈 텍스처로 변경
			}
		};

		//Albedo
		show_texture_slot("Albedo", albedo_texture, TextureType::Albedo);
		ImGui::SameLine();
		material_color_picker->Update();

		//Roughness
		show_texture_slot("Roughness", roughness_texture, TextureType::Roughness);
		ImGui::SameLine();
		ImGui::DragFloat("##Material_Roughness", &roughness, 0.004f, 0.0f, 1.0f);

		//Metallic
		show_texture_slot("Metallic", metallic_texture, TextureType::Metallic);
		ImGui::SameLine();
		ImGui::DragFloat("##Material_Metallic", &metallic, 0.004f, 0.0f, 1.0f);

		//Normal
		show_texture_slot("Normal", normal_texture, TextureType::Normal);
		ImGui::SameLine();
		ImGui::DragFloat("##Material_Normal", &normal, 0.004f, 0.0f, 1.0f);

		//Height
		show_texture_slot("Height", height_texture, TextureType::Height);
		ImGui::SameLine();
		ImGui::DragFloat("##Material_Height", &height, 0.004f, 0.0f, 1.0f);

		//Offset
		ImGui::TextUnformatted("Offset");
		ImGui::SameLine(70.0f);
		ImGui::TextUnformatted("X");
		ImGui::PushItemWidth(128.0f);
		ImGui::SameLine();
		ImGui::InputFloat("##Material_offset_x", &uv_offset.x, 0.01f, 0.1f, "%.2f", ImGuiInputTextFlags_CharsDecimal);
		ImGui::SameLine();
		ImGui::TextUnformatted("Y");
		ImGui::SameLine();
		ImGui::InputFloat("##Material_offset_y", &uv_offset.y, 0.01f, 0.1f, "%.2f", ImGuiInputTextFlags_CharsDecimal);
		ImGui::PopItemWidth();

		//Tiling
		ImGui::TextUnformatted("Tiling");
		ImGui::SameLine(70.0f);
		ImGui::TextUnformatted("X");
		ImGui::PushItemWidth(128.0f);
		ImGui::SameLine();
		ImGui::InputFloat("##Material_tiling_x", &uv_tiling.x, 0.01f, 0.1f, "%.2f", ImGuiInputTextFlags_CharsDecimal);
		ImGui::SameLine();
		ImGui::TextUnformatted("Y");
		ImGui::SameLine();
		ImGui::InputFloat("##Material_tiling_y", &uv_tiling.y, 0.01f, 0.1f, "%.2f", ImGuiInputTextFlags_CharsDecimal);
		ImGui::PopItemWidth();

		//게임모드가 아닐 경우 편집 가능
		if (!Engine::IsFlagEnabled(EngineFlags_Game))
		{
			if (roughness != material->GetRoughnessCoefficient()) material->SetRoughnessCoefficient(roughness);
			if (metallic != material->GetMetallicCoefficient())  material->SetMetallicCoefficient(metallic);
			if (normal != material->GetNormalCoefficient())    material->SetNormalCoefficient(normal);
			if (height != material->GetHeightCoefficient())    material->SetHeightCoefficient(height);
			if (uv_offset != material->GetOffset())               material->SetOffset(uv_offset);
			if (uv_tiling != material->GetTiling())               material->SetTiling(uv_tiling);
			if (material_color_picker->GetColor() != material->GetAlbedoColor())
				material->SetAlbedoColor(material_color_picker->GetColor());
		}
	}
	//컴퍼넌트를 구분할 구분 선 그리기
	Inspector_Data::ComponentEnd();
}


void Widget_Inspector::ShowLight(std::shared_ptr<class Light>& light) const
{
	if (!light.get())
		return;

	//Light 컴퍼넌트 요소 박스 만들기 
	if (Inspector_Data::ComponentBegin("Light", IconType::Component_Light, ComponentType::Light, light))
	{
		static std::vector<std::string> light_types
		{
			"Directional",
			"Point",
			"Spot",
		};

		//현재 설정된 light_type의 string값을 저장
		std::string light_type = light_types[static_cast<uint>(light->GetLightType())];

		auto intensity = light->GetIntensity();				//조명의 강도
		auto angle = Math::ToDegree(light->GetAngle());		//조명의 범위(각도)
		auto is_cast_shadow = light->IsCastShadow();		//그림자 설정(현재는 사용하지 않음)
		auto bias = light->GetBias();						//간접광 밝기
		auto normal_bias = light->GetNormalBias();			//간접광의 노말값
		auto range = light->GetRange();						//조명 거리
		
		light_color_picker->SetColor(light->GetColor());	 //ImGui에서 지원해주는 ColorPicker 사용(Light 전용)

		//Light Type
		ImGui::TextUnformatted("Type");
		ImGui::PushItemWidth(110.0f);
		ImGui::SameLine(140.0f);
		if (ImGui::BeginCombo("##LightType", light_type.c_str()))
		{
			for (uint i = 0; i < light_types.size(); i++)
			{
			    //현재 조명타입과 선택한 조명 타입이 같은 경우(true)
				const auto is_selected = light_type == light_types[i];
				if (ImGui::Selectable(light_types[i].c_str(), is_selected))
				{
					light_type = light_types[i];
					light->SetLightType(static_cast<LightType>(i));
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();

		//Light Color
		ImGui::TextUnformatted("Color");
		ImGui::SameLine(140.0f);
		light_color_picker->Update();

		//Light Intensity
		ImGui::TextUnformatted("Intensity");
		ImGui::SameLine(140.0f);
		ImGui::PushItemWidth(80.0f);
		ImGui::DragFloat("##Light_Intensity", &intensity, 0.1f, 0.0f, 100.0f);
		ImGui::PopItemWidth();

		//Light Cast Shadow
		ImGui::TextUnformatted("Is Cast Shadow");
		ImGui::SameLine(140.0f);
		ImGui::Checkbox("##Light_CastShadow", &is_cast_shadow);

		//Light Bias
		ImGui::TextUnformatted("Bias");
		ImGui::SameLine(140.0f);
		ImGui::PushItemWidth(120.0f);
		ImGui::InputFloat("##Light_Bias", &bias, 0.01f, 0.01f, "%.4f");
		ImGui::PopItemWidth();

		//Light Normal Bias
		ImGui::TextUnformatted("Normal Bias");
		ImGui::SameLine(140.0f);
		ImGui::PushItemWidth(120.0f);
		ImGui::InputFloat("##Light_Normal_Bias", &normal_bias, 1.0f, 1.0f, "%.0f");
		ImGui::PopItemWidth();

		//Light Range
		if (light->GetLightType() != LightType::Directional)
		{
			ImGui::TextUnformatted("Range");
			ImGui::SameLine(140.0f);
			ImGui::PushItemWidth(140.0f);
			ImGui::DragFloat("##Light_Range", &range, 0.01f, 0.0f, 100.0f);
			ImGui::PopItemWidth();
		}

		//Light Angle
		if (light->GetLightType() == LightType::Spot)
		{
			ImGui::TextUnformatted("Angle");
			ImGui::SameLine(140.0f);
			ImGui::PushItemWidth(140.0f);
			ImGui::DragFloat("##Light_Angle", &angle, 0.01f, 1.0f, 179.0f);
			ImGui::PopItemWidth();
		}

		//게임모드일 때에도 변경된 수치 적용
		if (intensity != light->GetIntensity())				  light->SetIntensity(intensity);
		if (is_cast_shadow != light->IsCastShadow())		  light->SetCastShadow(is_cast_shadow);
		if (bias != light->GetBias())						  light->SetBias(bias);
		if (normal_bias != light->GetNormalBias())			  light->SetNormalBias(normal_bias);
		if (range != light->GetRange())						  light->SetRange(range);
		if (Math::ToRadian(angle) != light->GetAngle())       light->SetAngle(Math::ToRadian(angle));

		if (light_color_picker->GetColor() != light->GetColor())
			light->SetColor(light_color_picker->GetColor());
	}
	//컴퍼넌트를 구분할 구분 선 그리기
	Inspector_Data::ComponentEnd();
}

//void Widget_Inspector::ShowTerrain(std::shared_ptr<class Terrain>& terrain) const
//{
//	if (!terrain.get())
//		return;
//
//	if (Inspector_Data::ComponentBegin("Terrain", IconType::Component_Terrain, ComponentType::Terrain, terrain, true, true))
//	{
//		auto texture = terrain->GetHeightMap();
//		auto min_y = terrain->GetMinY();
//		auto max_y = terrain->GetMaxY();
//		auto progress = terrain->GetProgress();
//
//		ImGui::BeginGroup();
//		{
//			ImGui::TextUnformatted("Height map");
//			ImGui::SameLine(90.0f);
//			ImGui::Image
//			(
//				texture ? texture->GetShaderResourceView() : nullptr,
//				ImVec2(80.0f, 80.0f),
//				ImVec2(0.0f, 0.0f),
//				ImVec2(1.0f, 1.0f),
//				ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
//				ImVec4(1.0f, 1.0f, 1.0f, 0.5f)
//			);
//
//			//Drag&Drop Event
//			if (auto payload = DragDropEvent::ReceiveDragDropPayload(PayloadType::Texture))
//			{
//					if (const auto resource = Editor_Helper::GetInstance()->resource_manager->Load<Texture2D>(std::get<const char*>(payload->data)))
//						terrain->SetHeightMap(resource);
//			}
//
//			//Remove button
//			if (terrain->HasHeightMap())
//			{
//				ImGui::SameLine();
//				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 30.0f);
//				if (Icon_Provider::Get().ImageButton("Terrain HeightMap", IconType::Button_Remove, 15.0f))
//					terrain->SetHeightMap(std::shared_ptr<Texture2D>());
//			}
//
//			if (ImGui::Button("Generate", ImVec2(83.0f, 0.0f)))
//				terrain->Generate();
//		}
//		ImGui::EndGroup();
//
//		ImGui::SameLine();
//
//		ImGui::BeginGroup();
//		{
//			ImGui::InputFloat("Min Y", &min_y);
//			ImGui::InputFloat("Max Y", &max_y);
//
//			if (progress > 0.0f && progress < 1.0f)
//			{
//				ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
//				ImGui::TextUnformatted(terrain->GetProgressState().c_str());
//			}
//		}
//		ImGui::EndGroup();
//
//		if (!Engine::IsFlagEnabled(EngineFlags_Game))
//		{
//			if (min_y != terrain->GetMinY())
//				terrain->SetMinY(min_y);
//			if (max_y != terrain->GetMaxY())
//				terrain->SetMaxY(max_y);
//		}
//	}
//	Inspector_Data::ComponentEnd();
//}

void Widget_Inspector::ShowAnimation(std::shared_ptr<class Animator>& animator)
{
	//애니메이션을 관리하는 애니메이터가 없는 경우 함수 탈출
	if (!animator.get())
		return;

	//Animation 컴퍼넌트 요소 박스 만들기 
	if (Inspector_Data::ComponentBegin("Animation", IconType::Component_Animator, ComponentType::Animator, animator, true, true))
	{
		//현재 선택된 엑터의 이름을 저장
		auto actor_name = Editor_Helper::GetInstance()->selected_actor.lock()->GetName();
		//ResourceManager에 현재 선택된 엑터와 같은 이름을 가진 모델이 존재한다면 수행
		if (auto actor_model = Editor_Helper::GetInstance()->resource_manager->GetResourceFromName<Model>(actor_name))
		{
			if (ImGui::Button("Animation List"))
			{
				ImGui::OpenPopup("##AnimationListPopup");
			}

			if (ImGui::BeginPopup("##AnimationListPopup"))
			{
				for (int i = 0; i < animator->GetAnimationCount(); ++i)
				{
					auto animation_name = animator->GetAnimationName(i);
					//애니메이션 리스트에서 클릭한 애니메이션을 현재 애니메이션 상태로 변경 
					if (ImGui::MenuItem(animation_name.c_str()))
					{
						animator->SetCurrentAnimation(animation_name);
					}
				}
				ImGui::EndPopup();
			}

			//Add Animation버튼을 누른 횟수만큼 Animation Editor를 보여줌
			for (int i = 0; i < actor_model->GetAnimationCount(); ++i)
			{
				auto animation_name = animator->GetAnimationName(i);

				ImGui::TextUnformatted("Animation");
				ImGui::SameLine(100.0f);
				ImGui::PushItemWidth(250.0f);
				ImGui::InputText("##AnimationName", &animation_name, ImGuiInputTextFlags_ReadOnly);
				ImGui::PopItemWidth();

				//게임모드가 아닐 경우(편집모드)에만 사용가능
				if (!Engine::IsFlagEnabled(EngineFlags_Game))
				{
					//Drag&Drop Event
			        //Payload Data(Animation)
					if (auto payload = DragDropEvent::ReceiveDragDropPayload(PayloadType::Model))
					{
						actor_model->LoadFromFile(std::get<const char*>(payload->data)); //Load Payload(Animation)
					}
				}
			}
		}
		ShowAddAnimationButton();
	}
	//컴퍼넌트를 구분할 구분 선 그리기
	Inspector_Data::ComponentEnd();
}

void Widget_Inspector::ShowAddAnimationButton()
{
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 50.0f);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
	//게임모드가 아닐 경우(편집모드)에만 사용가능
	if (!Engine::IsFlagEnabled(EngineFlags_Game))
	{
		if (ImGui::Button("Add Animation"))
		{
			//현재 선택된 엑터의 이름을 저장
			auto actor_name = Editor_Helper::GetInstance()->selected_actor.lock()->GetName();
			//선택된 엑터와 같은 이름을 가진 모델을 저장
			auto actor_model = Editor_Helper::GetInstance()->resource_manager->GetResourceFromName<Model>(actor_name);

			uint animation_count = actor_model->GetAnimationCount();
			animation_count++; //애니메이션을 추가할 칸을 증가
			actor_model->SetAnimationCount(animation_count);
		}
	}
}

void Widget_Inspector::ShowScript(std::shared_ptr<class Script>& script) const
{
	if (!script.get())
		return;

	//Script 컴퍼넌트 요소 박스 만들기 
	if (Inspector_Data::ComponentBegin(script->GetScriptName(), IconType::Component_Script, ComponentType::Script, script, true, true))
	{
		auto script_name = script->GetScriptName();

		ImGui::TextUnformatted("Script");
		ImGui::SameLine(70.0f);
		ImGui::PushID("##ScriptName");
		ImGui::PushItemWidth(200.0f);
		{
			ImGui::InputText("", &script_name, ImGuiInputTextFlags_ReadOnly);

			//Drag&Drop Event
			//Payload Data(Script)
			if (auto payload = DragDropEvent::ReceiveDragDropPayload(PayloadType::Script))
				script->SetScript(std::get<const char*>(payload->data)); //Set Payload(Script) 

			ImGui::SameLine();

			//편집 버튼을 누르면
			if (ImGui::Button("Edit"))
			{
				ScriptEditor::Get().SetScript(script->GetScriptPath());
				ScriptEditor::Get().SetVisible(true); //스크립트 보여주기
			}
		}
		ImGui::PopItemWidth();
		ImGui::PopID();
	}
	//컴퍼넌트를 구분할 구분 선 그리기
	Inspector_Data::ComponentEnd();
}

void Widget_Inspector::ShowAudioSource(std::shared_ptr<class AudioSource>& audio_source)
{
	if (!audio_source.get())
		return;

	//AudioSource 컴퍼넌트 요소 박스 만들기 
	if (Inspector_Data::ComponentBegin("AudioSource", IconType::Component_AudioSource, ComponentType::AudioSource, audio_source, true, true))
	{
		auto audio_clip_name = audio_source->GetAudioClipName();
		auto is_playing = audio_source->IsPlaying(); //해당 음원이 재생 중인지
		auto is_loop = audio_source->IsLoop();		 //해당 음원의 재생이 끝나고 다시 처음부터 재생시킬지
		auto is_mute = audio_source->IsMute();		 //해당 음원을 중지 시킬지
		auto priority = audio_source->GetPriority(); //해당 음원의 우선순위(낮을 수록 높은 우선 순위를 가짐)
		auto volume = audio_source->GetVolume();	 //해당 음원의 볼륨 값
		auto pitch = audio_source->GetPitch();		 //해당 음원의 음의 높낮이
		auto pan = audio_source->GetPan();			 //해당 음원의 좌우 위치에서 들리는 각각의 소리를 조절하여 특정 위치에서 들리는 듯한 느낌을 주는 옵션 (범위: -1 ~ 1)
		 
		//Audio Clip
		ImGui::TextUnformatted("Audio Clip");
		ImGui::SameLine(140.0f);
		ImGui::PushItemWidth(250.0f);
		ImGui::InputText("##AudioClipName", &audio_clip_name, ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();

		//Drag&Drop Event
		//Payload Data(Audio)
		if (auto payload = DragDropEvent::ReceiveDragDropPayload(PayloadType::Audio))
			audio_source->SetAudioClip(std::get<const char*>(payload->data)); //Set Payload(Audio) 

		//Playing
		ImGui::TextUnformatted("Is Playing");
		ImGui::SameLine(140.0f);
		ImGui::Checkbox("##AudioPlaying", &is_playing);

		//Loop
		ImGui::TextUnformatted("Is Loop");
		ImGui::SameLine(140.0f);
		ImGui::Checkbox("##AudioLoop", &is_loop);

		//Mute
		ImGui::TextUnformatted("Is Mute");
		ImGui::SameLine(140.0f);
		ImGui::Checkbox("##AudioMute", &is_mute);

		//Priority
		ImGui::TextUnformatted("Priority");
		ImGui::SameLine(140.0f);
		ImGui::SliderInt("##AudioPriority", &priority, 0, 255);

		//Volume
		ImGui::TextUnformatted("Volume");
		ImGui::SameLine(140.0f);
		ImGui::SliderFloat("##AudioVolume", &volume, 0.0f, 1.0f);

		//Pitch
		ImGui::TextUnformatted("Pitch");
		ImGui::SameLine(140.0f);
		ImGui::SliderFloat("##AudioPitch", &pitch, 0.0f, 3.0f);

		//Pan
		ImGui::TextUnformatted("Pan");
		ImGui::SameLine(140.0f);
		ImGui::SliderFloat("##AudioPan", &pan, -1.0f, 1.0f);

		//게임모드일 때에도 변경된 수치 적용
		if (is_playing != audio_source->IsPlaying())   audio_source->SetPlaying(is_playing);
		if (is_loop != audio_source->IsLoop())      audio_source->SetLoop(is_loop);
		if (is_mute != audio_source->IsMute())      audio_source->SetMute(is_mute);
		if (priority != audio_source->GetPriority()) audio_source->SetPriority(priority);
		if (volume != audio_source->GetVolume())   audio_source->SetVolume(volume);
		if (pitch != audio_source->GetPitch())    audio_source->SetPitch(pitch);
		if (pan != audio_source->GetPan())      audio_source->SetPan(pan);
	}
	//컴퍼넌트를 구분할 구분 선 그리기
	Inspector_Data::ComponentEnd();
}

void Widget_Inspector::ShowAudioListener(std::shared_ptr<class AudioListener>& audio_listener)
{
	if (!audio_listener.get())
		return;

	//AudioListener 컴퍼넌트 요소 박스 만들기 
	Inspector_Data::ComponentBegin("AudioListener", IconType::Component_AudioListener, ComponentType::AudioListener, audio_listener, true, true);
	//컴퍼넌트를 구분할 구분 선 그리기
	Inspector_Data::ComponentEnd();
}

//AddComponent Button 출력
void Widget_Inspector::ShowAddComponentButton()
{
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 50.0f);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("##ComponentPopup");

	ShowComponentPopup();
}

//AddComponent Button 클릭 후 출력되는 Button들 출력
void Widget_Inspector::ShowComponentPopup()
{
	if (ImGui::BeginPopup("##ComponentPopup"))
	{
		if (auto actor = Editor_Helper::GetInstance()->selected_actor.lock())
		{
			//Light
			if (ImGui::BeginMenu("Light"))
			{
				if (ImGui::MenuItem("Directional")) //Directional
					actor->AddComponent<Light>()->SetLightType(LightType::Directional);
				if (ImGui::MenuItem("Point")) //Point
					actor->AddComponent<Light>()->SetLightType(LightType::Point);
				if (ImGui::MenuItem("Spot")) //Spot
					actor->AddComponent<Light>()->SetLightType(LightType::Spot);
				ImGui::EndMenu();
			}

			//Animation
			if (ImGui::MenuItem("Animation")) //Animation
				actor->AddComponent<Animator>();

			//Script
			if (ImGui::MenuItem("Script")) //Script
				actor->AddComponent<Script>();

			//Audio
			if (ImGui::BeginMenu("Audio"))
			{
				if (ImGui::MenuItem("AudioSource")) //AudioSource
					actor->AddComponent<AudioSource>();
				if (ImGui::MenuItem("AudioListener")) //AudioListener
					actor->AddComponent<AudioListener>();
				ImGui::EndMenu();
			}
		}
		ImGui::EndPopup();
	}
}
