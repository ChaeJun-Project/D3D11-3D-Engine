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

//�߰��� ���۳�Ʈ�� ������ �ڽ��� Ư�� ���۳�Ʈ�� ������ �� �ֵ��� ��� ����
namespace Inspector_Data
{
	static std::string popup_name;

	//Ư�� ���۳�Ʈ �ɼ� �˾�
	inline void ComponentMenuPopup(const std::string& popup_id, const ComponentType& component_type, const std::shared_ptr<IComponent>& component, const bool& is_removeable)
	{
		if (ImGui::BeginPopup(popup_id.c_str()))
		{
			if (is_removeable)
			{
				//���� ��ư�� ������ ���
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

	//Ư�� ���۳�Ʈ ��� �ڽ� ����� 
	inline const bool ComponentBegin(const std::string& name, const IconType& type, const ComponentType& component_type, const std::shared_ptr<IComponent>& component, const bool& is_option = true, const bool& is_removeable = true)
	{
		//���۳�Ʈ ��� �ڽ��� ��� �����
		const auto collapsed_header = ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen);

		ImGui::SameLine(); //���۳�Ʈ ����� ���� ����
		ImGui::Spacing(); //���۳�Ʈ Ÿ��Ʋ ���� �� ���� ����� 
		ImGui::SameLine(); //���۳�Ʈ ����� ���� ����

		const auto original_pen_y = ImGui::GetCursorPosY();

		ImGui::SetCursorPosY(original_pen_y + 4.0f); //���� UI�� �׸� y�ຸ�� ������ 4��ŭ �̵�

		//���۳�Ʈ ������ �����
		Icon_Provider::Get().Image(type, 15.0f);

		if (is_option)
		{
			ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 15.0f);
			ImGui::SetCursorPosY(original_pen_y);

			//�ɼ� ������&��ư �����
			if (Icon_Provider::Get().ImageButton(name.c_str(), IconType::Button_Option, 12.0f))
			{
				Inspector_Data::popup_name = name;
				//���۳�Ʈ�� �̸����� ���۳�Ʈ ���� �˾� �����
				ImGui::OpenPopup(Inspector_Data::popup_name.c_str());
			}

			//���� ���۳�Ʈ �ɼ� ��ư�� ���� ���۳�Ʈ�� ���� ����� �˾��� �̸��� ���� ���
			//���۳�Ʈ ���� �˾� ����ֱ�
			if (Inspector_Data::popup_name == name)
				//Component ���� �޴� ����
				ComponentMenuPopup(Inspector_Data::popup_name, component_type, component, is_removeable);
		}

		return collapsed_header;
	}

	//���۳�Ʈ�� ������ ���� �� �׸���
	inline void ComponentEnd()
	{
		ImGui::Separator();
	}
}

Widget_Inspector::Widget_Inspector(Context * context)
	:IWidget(context)
{
	//�ش� GUI Ÿ��Ʋ �ʱ�ȭ
	title = "Inspector";

	//ImGui���� �������ִ� ColorPicker ���(Material ����)
	material_color_picker = std::make_unique<ColorPicker>("Material Color Picker");

	//ImGui���� �������ִ� ColorPicker ���(Light ����)
	light_color_picker = std::make_unique<ColorPicker>("Light Color Picker");
}

void Widget_Inspector::Render()
{
	//�� ���۳�Ʈ â�� �ʺ� 100���� ����
	ImGui::PushItemWidth(max_width);
	{
		if (auto actor = Editor_Helper::GetInstance()->selected_actor.lock())
		{
			//���õ� Actor�� Transform Component�� ���Ե� ���
			if (auto transform = actor->GetComponent<Transform>())
				ShowTransform(transform); //Transform Component �����ֱ�

			//���õ� Actor�� Material Component�� ���Ե� ���
			if (auto renderable = actor->GetComponent<Renderable>())
			{
				if (auto material = renderable->GetMaterial())
					ShowMaterial(material); //Material Component �����ֱ�
			}

			//���õ� Actor�� Light Component�� ���Ե� ���
			if (auto light = actor->GetComponent<Light>())
				ShowLight(light); //Light Component �����ֱ�

			/*if (auto terrain = actor->GetComponent<Terrain>())
			ShowTerrain(terrain);*/

			//���õ� Actor�� Animator Component�� ���Ե� ���
			if (auto animator = actor->GetComponent<Animator>())
				ShowAnimation(animator); //Animaton Component �����ֱ�

			//���õ� Actor�� Script Component�� ���Ե� ���
			if (auto script = actor->GetComponent<Script>())
				ShowScript(script); //Script Component �����ֱ�

			//���õ� Actor�� AudioSource Component�� ���Ե� ���
			if (auto audio_source = actor->GetComponent<AudioSource>())
				ShowAudioSource(audio_source); //AudioSource Component �����ֱ�

			//���õ� Actor�� AudioListener Component�� ���Ե� ���
			if (auto audio_listener = actor->GetComponent<AudioListener>())
				ShowAudioListener(audio_listener); //AudioListener Component �����ֱ�
		}

		//Add Component Button ������
		ShowAddComponentButton();
	}
	//�� ���۳�Ʈ â�� �ʺ� 100���� ���� Ǯ��
	ImGui::PopItemWidth();
}

void Widget_Inspector::ShowTransform(std::shared_ptr<class Transform>& transform) const
{
	if (!transform.get())
		return;

	//Transform ���۳�Ʈ ��� �ڽ� ����� 
	if (Inspector_Data::ComponentBegin("Transform", IconType::Component_Transform, ComponentType::Transform, transform, false, false))
	{
		auto position = transform->GetTranslation();
		auto rotation = transform->GetRotation().ToEulerAngle();
		auto scale = transform->GetScale();

		//���� �Լ� show_float
		//���ٸ� ���� ������ ���ȭ �Ǿ�� �ϹǷ� const�� 
		const auto show_float = [](const char* label, float* value)
		{
			ImGui::TextUnformatted(label);
			ImGui::SameLine();

			ImGui::PushItemWidth(60.0f);
			ImGui::PushID(static_cast<int>(ImGui::GetCursorPosX() + ImGui::GetCursorPosY())); //��ġ�� ���� ID�� ����
			 //�ش� Input Box�� �巡���Ͽ� 
			 //value���� float Ÿ���� �ּ�,�ִ밪���� ������ �� �ֵ��� ����
			ImGui::DragFloat("##no_label", value, 0.1f, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());
			ImGui::PopID();
			ImGui::PopItemWidth();
		};

		//���� �Լ� show_vector3
		//show_float ���� �Լ� ĸó
		const auto show_vector3 = [&show_float](const char* label, Vector3& value)
		{
			ImGui::BeginGroup(); //�ϳ��� �׷����� ����
			ImGui::Indent(15.0f); //�鿩����
			ImGui::TextUnformatted(label); //��(Position, Rotation, Scale)
			show_float("X", &value.x); ImGui::SameLine(100.0f); //���� ���ο� ���ۺ��� 100��ŭ ������ ������
			show_float("Y", &value.y); ImGui::SameLine(185.0f); //���� ���ο� ���ۺ��� 185��ŭ ������ ������
			show_float("Z", &value.z);
			ImGui::EndGroup(); //�ϳ��� �׷����� ���� ����
		};

		show_vector3("Position", position); //Position
		show_vector3("Rotation", rotation); //Rotation
		show_vector3("Scale", scale);		//Scale

		//���Ӹ�尡 �ƴ� ��� ���� ����
		if (!Engine::IsFlagEnabled(EngineFlags_Game))
		{
			transform->SetTranslation(position); //Position �缳��
			transform->SetRotation(Quaternion::QuaternionFromEulerAngle(rotation)); //Rotation �缳��
			transform->SetScale(scale); //Scale �缳��
		}
	}
	//���۳�Ʈ�� ������ ���� �� �׸���
	Inspector_Data::ComponentEnd();
}

void Widget_Inspector::ShowMaterial(std::shared_ptr<class Material>& material) const
{
	if (!material.get())
		return;

	//Material ���۳�Ʈ ��� �ڽ� ����� 
	if (Inspector_Data::ComponentBegin("Material", IconType::Component_Material, ComponentType::Unknown, nullptr, false, false))
	{
		auto roughness = material->GetRoughnessCoefficient(); //��ģ����(��ġ)
		auto metallic = material->GetMetallicCoefficient();   //�ݼ�����(��ġ)
		auto normal = material->GetNormalCoefficient();		  //��ü����(��ġ)
		auto height = material->GetHeightCoefficient();		  //��������(��ġ)
		auto uv_offset = material->GetOffset();				  //�ؽ�ó ������(������)(��ġ)
		auto uv_tiling = material->GetTiling();				  //�ؽ�ó Ÿ�ϸ�(��ġ)

		material_color_picker->SetColor(material->GetAlbedoColor()); //ImGui���� �������ִ� ColorPicker ���(Material ����)

		const auto albedo_texture = material->GetTexture(TextureType::Albedo);			//��ü�� ���� �ܼ��� ��
		const auto roughness_texture = material->GetTexture(TextureType::Roughness);	//ǥ���� ��ģ����
		const auto metallic_texture = material->GetTexture(TextureType::Metallic);		//ǥ���� �󸶳� �ݼӰ��� ��
		const auto normal_texture = material->GetTexture(TextureType::Normal);			//ǥ���� ��ü���� ǥ��
		const auto height_texture = material->GetTexture(TextureType::Height);			//ǥ���� y���� �̹��� ���Ϸ� ������ ��

		//���� �Լ� show_texture_slot
		//������Ʈ�� ������ �ؽ�ó�� Inspectorâ���� �̹����� �����ִ� �Լ�
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
				if (Icon_Provider::Get().ImageButton(name, IconType::Button_Remove, 15.0f)) //���� ��ư�� ������ ��
					material->SetTexture(type, std::shared_ptr<ITexture>()); //�� �ؽ�ó�� ����
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

		//���Ӹ�尡 �ƴ� ��� ���� ����
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
	//���۳�Ʈ�� ������ ���� �� �׸���
	Inspector_Data::ComponentEnd();
}


void Widget_Inspector::ShowLight(std::shared_ptr<class Light>& light) const
{
	if (!light.get())
		return;

	//Light ���۳�Ʈ ��� �ڽ� ����� 
	if (Inspector_Data::ComponentBegin("Light", IconType::Component_Light, ComponentType::Light, light))
	{
		static std::vector<std::string> light_types
		{
			"Directional",
			"Point",
			"Spot",
		};

		//���� ������ light_type�� string���� ����
		std::string light_type = light_types[static_cast<uint>(light->GetLightType())];

		auto intensity = light->GetIntensity();				//������ ����
		auto angle = Math::ToDegree(light->GetAngle());		//������ ����(����)
		auto is_cast_shadow = light->IsCastShadow();		//�׸��� ����(����� ������� ����)
		auto bias = light->GetBias();						//������ ���
		auto normal_bias = light->GetNormalBias();			//�������� �븻��
		auto range = light->GetRange();						//���� �Ÿ�
		
		light_color_picker->SetColor(light->GetColor());	 //ImGui���� �������ִ� ColorPicker ���(Light ����)

		//Light Type
		ImGui::TextUnformatted("Type");
		ImGui::PushItemWidth(110.0f);
		ImGui::SameLine(140.0f);
		if (ImGui::BeginCombo("##LightType", light_type.c_str()))
		{
			for (uint i = 0; i < light_types.size(); i++)
			{
			    //���� ����Ÿ�԰� ������ ���� Ÿ���� ���� ���(true)
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

		//���Ӹ���� ������ ����� ��ġ ����
		if (intensity != light->GetIntensity())				  light->SetIntensity(intensity);
		if (is_cast_shadow != light->IsCastShadow())		  light->SetCastShadow(is_cast_shadow);
		if (bias != light->GetBias())						  light->SetBias(bias);
		if (normal_bias != light->GetNormalBias())			  light->SetNormalBias(normal_bias);
		if (range != light->GetRange())						  light->SetRange(range);
		if (Math::ToRadian(angle) != light->GetAngle())       light->SetAngle(Math::ToRadian(angle));

		if (light_color_picker->GetColor() != light->GetColor())
			light->SetColor(light_color_picker->GetColor());
	}
	//���۳�Ʈ�� ������ ���� �� �׸���
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
	//�ִϸ��̼��� �����ϴ� �ִϸ����Ͱ� ���� ��� �Լ� Ż��
	if (!animator.get())
		return;

	//Animation ���۳�Ʈ ��� �ڽ� ����� 
	if (Inspector_Data::ComponentBegin("Animation", IconType::Component_Animator, ComponentType::Animator, animator, true, true))
	{
		//���� ���õ� ������ �̸��� ����
		auto actor_name = Editor_Helper::GetInstance()->selected_actor.lock()->GetName();
		//ResourceManager�� ���� ���õ� ���Ϳ� ���� �̸��� ���� ���� �����Ѵٸ� ����
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
					//�ִϸ��̼� ����Ʈ���� Ŭ���� �ִϸ��̼��� ���� �ִϸ��̼� ���·� ���� 
					if (ImGui::MenuItem(animation_name.c_str()))
					{
						animator->SetCurrentAnimation(animation_name);
					}
				}
				ImGui::EndPopup();
			}

			//Add Animation��ư�� ���� Ƚ����ŭ Animation Editor�� ������
			for (int i = 0; i < actor_model->GetAnimationCount(); ++i)
			{
				auto animation_name = animator->GetAnimationName(i);

				ImGui::TextUnformatted("Animation");
				ImGui::SameLine(100.0f);
				ImGui::PushItemWidth(250.0f);
				ImGui::InputText("##AnimationName", &animation_name, ImGuiInputTextFlags_ReadOnly);
				ImGui::PopItemWidth();

				//���Ӹ�尡 �ƴ� ���(�������)���� ��밡��
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
	//���۳�Ʈ�� ������ ���� �� �׸���
	Inspector_Data::ComponentEnd();
}

void Widget_Inspector::ShowAddAnimationButton()
{
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 50.0f);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
	//���Ӹ�尡 �ƴ� ���(�������)���� ��밡��
	if (!Engine::IsFlagEnabled(EngineFlags_Game))
	{
		if (ImGui::Button("Add Animation"))
		{
			//���� ���õ� ������ �̸��� ����
			auto actor_name = Editor_Helper::GetInstance()->selected_actor.lock()->GetName();
			//���õ� ���Ϳ� ���� �̸��� ���� ���� ����
			auto actor_model = Editor_Helper::GetInstance()->resource_manager->GetResourceFromName<Model>(actor_name);

			uint animation_count = actor_model->GetAnimationCount();
			animation_count++; //�ִϸ��̼��� �߰��� ĭ�� ����
			actor_model->SetAnimationCount(animation_count);
		}
	}
}

void Widget_Inspector::ShowScript(std::shared_ptr<class Script>& script) const
{
	if (!script.get())
		return;

	//Script ���۳�Ʈ ��� �ڽ� ����� 
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

			//���� ��ư�� ������
			if (ImGui::Button("Edit"))
			{
				ScriptEditor::Get().SetScript(script->GetScriptPath());
				ScriptEditor::Get().SetVisible(true); //��ũ��Ʈ �����ֱ�
			}
		}
		ImGui::PopItemWidth();
		ImGui::PopID();
	}
	//���۳�Ʈ�� ������ ���� �� �׸���
	Inspector_Data::ComponentEnd();
}

void Widget_Inspector::ShowAudioSource(std::shared_ptr<class AudioSource>& audio_source)
{
	if (!audio_source.get())
		return;

	//AudioSource ���۳�Ʈ ��� �ڽ� ����� 
	if (Inspector_Data::ComponentBegin("AudioSource", IconType::Component_AudioSource, ComponentType::AudioSource, audio_source, true, true))
	{
		auto audio_clip_name = audio_source->GetAudioClipName();
		auto is_playing = audio_source->IsPlaying(); //�ش� ������ ��� ������
		auto is_loop = audio_source->IsLoop();		 //�ش� ������ ����� ������ �ٽ� ó������ �����ų��
		auto is_mute = audio_source->IsMute();		 //�ش� ������ ���� ��ų��
		auto priority = audio_source->GetPriority(); //�ش� ������ �켱����(���� ���� ���� �켱 ������ ����)
		auto volume = audio_source->GetVolume();	 //�ش� ������ ���� ��
		auto pitch = audio_source->GetPitch();		 //�ش� ������ ���� ������
		auto pan = audio_source->GetPan();			 //�ش� ������ �¿� ��ġ���� �鸮�� ������ �Ҹ��� �����Ͽ� Ư�� ��ġ���� �鸮�� ���� ������ �ִ� �ɼ� (����: -1 ~ 1)
		 
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

		//���Ӹ���� ������ ����� ��ġ ����
		if (is_playing != audio_source->IsPlaying())   audio_source->SetPlaying(is_playing);
		if (is_loop != audio_source->IsLoop())      audio_source->SetLoop(is_loop);
		if (is_mute != audio_source->IsMute())      audio_source->SetMute(is_mute);
		if (priority != audio_source->GetPriority()) audio_source->SetPriority(priority);
		if (volume != audio_source->GetVolume())   audio_source->SetVolume(volume);
		if (pitch != audio_source->GetPitch())    audio_source->SetPitch(pitch);
		if (pan != audio_source->GetPan())      audio_source->SetPan(pan);
	}
	//���۳�Ʈ�� ������ ���� �� �׸���
	Inspector_Data::ComponentEnd();
}

void Widget_Inspector::ShowAudioListener(std::shared_ptr<class AudioListener>& audio_listener)
{
	if (!audio_listener.get())
		return;

	//AudioListener ���۳�Ʈ ��� �ڽ� ����� 
	Inspector_Data::ComponentBegin("AudioListener", IconType::Component_AudioListener, ComponentType::AudioListener, audio_listener, true, true);
	//���۳�Ʈ�� ������ ���� �� �׸���
	Inspector_Data::ComponentEnd();
}

//AddComponent Button ���
void Widget_Inspector::ShowAddComponentButton()
{
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 0.5f - 50.0f);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 5.0f);
	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("##ComponentPopup");

	ShowComponentPopup();
}

//AddComponent Button Ŭ�� �� ��µǴ� Button�� ���
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
