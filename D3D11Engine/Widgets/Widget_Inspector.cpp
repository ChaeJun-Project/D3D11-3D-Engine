#include "stdafx.h"
#include "Widget_Inspector.h"
#include "Scene/Actor.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/Light.h"
#include "Scene/Component/Renderable.h"
#include "Scene/Component/Terrain.h"
#include "Scene/Component/Script.h"
#include "Scene/Component/AudioSource.h"
#include "Scene/Component/AudioListener.h"
#include "Scene/Component/Animator.h"

namespace Inspector_Data
{
	static std::shared_ptr<IComponent> copied_component;
	static std::string popup_name;
	static float column = 140.0f;
	static float max_width = 100.0f;

	inline void ComponentMenuPopup(const std::string& popup_id, const ComponentType& component_type, const std::shared_ptr<IComponent>& component, const bool& is_removeable)
	{
		if (ImGui::BeginPopup(popup_id.c_str()))
		{
			if (is_removeable)
			{
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

	inline const bool ComponentBegin(const std::string& name, const IconType& type, const ComponentType& component_type, const std::shared_ptr<IComponent>& component, const bool& is_option = true, const bool& is_removeable = true)
	{
		const auto collapsed_header = ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen);

		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();

		const auto original_pen_y = ImGui::GetCursorPosY();
		ImGui::SetCursorPosY(original_pen_y + 5.0f);

		Icon_Provider::Get().Image(type, 15.0f);

		if (is_option)
		{
			ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 15.0f);
			ImGui::SetCursorPosY(original_pen_y);

			if (Icon_Provider::Get().ImageButton(name.c_str(), IconType::Button_Option, 12.0f))
			{
				Inspector_Data::popup_name = name;
				ImGui::OpenPopup(Inspector_Data::popup_name.c_str());
			}

			if (Inspector_Data::popup_name == name)
				//Component ���� �޴� ����
				ComponentMenuPopup(Inspector_Data::popup_name, component_type, component, is_removeable);
		}

		return collapsed_header;
	}

	inline void ComponentEnd()
	{
		ImGui::Separator();
	}
}

Widget_Inspector::Widget_Inspector(Context * context)
	:IWidget(context)
{
	title = "Inspector";

	material_color_picker = std::make_unique<ColorPicker>("Material Color Picker");
	light_color_picker = std::make_unique<ColorPicker>("Light Color Picker");
}

void Widget_Inspector::Render()
{
	ImGui::PushItemWidth(Inspector_Data::max_width);

	if (auto actor = Editor_Helper::GetInstance()->selected_actor.lock())
	{
		auto transform = actor->GetComponent<Transform>();
		ShowTransform(transform);

		auto material = actor->GetComponent<Renderable>()->GetMaterial();
		ShowMaterial(material);

		auto light = actor->GetComponent<Light>();
		ShowLight(light);

		/*auto terrain = actor->GetComponent<Terrain>();
		ShowTerrain(terrain);*/

		auto animator = actor->GetComponent<Animator>();
		ShowAnimation(animator);

		auto script = actor->GetComponent<Script>();
		ShowScript(script);

		auto audio_source = actor->GetComponent<AudioSource>();
		ShowAudioSource(audio_source);

		auto audio_listener = actor->GetComponent<AudioListener>();
		ShowAudioListener(audio_listener);
	}

	ShowAddComponentButton();

	ImGui::PopItemWidth();
}

void Widget_Inspector::ShowTransform(std::shared_ptr<class Transform>& transform) const
{
	if (!transform)
		return;

	if (Inspector_Data::ComponentBegin("Transform", IconType::Component_Transform, ComponentType::Transform, transform, false, false))
	{
		auto position = transform->GetTranslation();
		auto rotation = transform->GetRotation().ToEulerAngle();
		auto scale = transform->GetScale();

		const auto show_float = [](const char* label, float* value)
		{
			ImGui::TextUnformatted(label);
			ImGui::SameLine();

			ImGui::PushItemWidth(60.0f);
			ImGui::PushID(static_cast<int>(ImGui::GetCursorPosX() + ImGui::GetCursorPosY()));
			ImGui::DragFloat("##no_label", value, 0.1f, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());
			ImGui::PopID();
			ImGui::PopItemWidth();
		};

		const auto show_vector3 = [&show_float](const char* label, Vector3& value)
		{
			ImGui::BeginGroup();
			ImGui::Indent(15.0f); //�鿩����
			ImGui::TextUnformatted(label); //��(Position, Rotation, Scale)
			show_float("X", &value.x); ImGui::SameLine(100.0f); //���� ���ο� ���ۺ��� 100��ŭ ������ ������
			show_float("Y", &value.y); ImGui::SameLine(185.0f); //���� ���ο� ���ۺ��� 185��ŭ ������ ������
			show_float("Z", &value.z);
			ImGui::EndGroup();
		};

		show_vector3("Position", position);
		show_vector3("Rotation", rotation);
		show_vector3("Scale", scale);

		//���Ӹ�尡 �ƴ� ���
		if (!Engine::IsFlagEnabled(EngineFlags_Game))
		{
			transform->SetTranslation(position);
			transform->SetRotation(Quaternion::QuaternionFromEulerAngle(rotation));
			transform->SetScale(scale);
		}
	}
	Inspector_Data::ComponentEnd();
}

void Widget_Inspector::ShowMaterial(std::shared_ptr<class Material>& material) const
{
	if (!material)
		return;

	if (Inspector_Data::ComponentBegin("Material", IconType::Component_Material, ComponentType::Unknown, nullptr, false, false))
	{
		auto roughness = material->GetRoughnessCoefficient();
		auto metallic = material->GetMetallicCoefficient();
		auto normal = material->GetNormalCoefficient();
		auto height = material->GetHeightCoefficient();
		auto uv_offset = material->GetOffset();
		auto uv_tiling = material->GetTiling();
		material_color_picker->SetColor(material->GetAlbedoColor());

		const auto albedo_texture = material->GetTexture(TextureType::Albedo);
		const auto roughness_texture = material->GetTexture(TextureType::Roughness);
		const auto metallic_texture = material->GetTexture(TextureType::Metallic);
		const auto normal_texture = material->GetTexture(TextureType::Normal);
		const auto height_texture = material->GetTexture(TextureType::Height);
		const auto occlusion_texture = material->GetTexture(TextureType::Occlusion);
		const auto emissive_texture = material->GetTexture(TextureType::Emissive);
		const auto mask_texture = material->GetTexture(TextureType::Mask);

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

			//Drag Drop Event
			if (auto payload = DragDropEvent::ReceiveDragDropPayload(PayloadType::Texture))
			{
				try
				{
					if (const auto resource = Editor_Helper::GetInstance()->resource_manager->Load<Texture2D>(std::get<const char*>(payload->data)))
						material->SetTexture(type, resource);
				}
				catch (const std::bad_variant_access& error)
				{
					LOG_ERROR_F("%s", error.what());
				}
			}

			//Remove Button
			if (material->HasTexture(type))
			{
				ImGui::SameLine();
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 30.0f);
				if (Icon_Provider::Get().ImageButton(name, IconType::Button_Remove, 15.0f))
					material->SetTexture(type, std::shared_ptr<ITexture>());
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

		//Occlusion
		show_texture_slot("Occlusion", occlusion_texture, TextureType::Occlusion);

		//Emissive
		show_texture_slot("Emissive", emissive_texture, TextureType::Emissive);

		//Mask
		show_texture_slot("Mask", mask_texture, TextureType::Mask);

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
	Inspector_Data::ComponentEnd();
}


void Widget_Inspector::ShowLight(std::shared_ptr<class Light>& light) const
{
	if (!light)
		return;

	if (Inspector_Data::ComponentBegin("Light", IconType::Component_Light, ComponentType::Light, light))
	{
		static std::vector<std::string> light_types
		{
			"Directional",
			"Point",
			"Spot",
		};

		std::string light_type = light_types[static_cast<uint>(light->GetLightType())];

		auto intensity = light->GetIntensity();
		auto angle = Math::ToDegree(light->GetAngle());
		auto is_cast_shadow = light->IsCastShadow();
		auto bias = light->GetBias();
		auto normal_bias = light->GetNormalBias();
		auto range = light->GetRange();
		light_color_picker->SetColor(light->GetColor());

		//Light Type
		ImGui::TextUnformatted("Type");
		ImGui::PushItemWidth(110.0f);
		ImGui::SameLine(140.0f);
		if (ImGui::BeginCombo("##LightType", light_type.c_str()))
		{
			for (uint i = 0; i < light_types.size(); i++)
			{
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

		if (intensity != light->GetIntensity())   light->SetIntensity(intensity);
		if (is_cast_shadow != light->IsCastShadow())   light->SetCastShadow(is_cast_shadow);
		if (bias != light->GetBias())        light->SetBias(bias);
		if (normal_bias != light->GetNormalBias())  light->SetNormalBias(normal_bias);
		if (range != light->GetRange())       light->SetRange(range);
		if (Math::ToRadian(angle) != light->GetAngle())       light->SetAngle(Math::ToRadian(angle));

		if (light_color_picker->GetColor() != light->GetColor())
			light->SetColor(light_color_picker->GetColor());
	}
	Inspector_Data::ComponentEnd();
}

//void Widget_Inspector::ShowTerrain(std::shared_ptr<class Terrain>& terrain) const
//{
//	if (!terrain)
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
//			//Drag Drop Event
//			if (auto payload = DragDropEvent::ReceiveDragDropPayload(PayloadType::Texture))
//			{
//				try
//				{
//					if (const auto resource = Editor_Helper::GetInstance()->resource_manager->Load<Texture2D>(std::get<const char*>(payload->data)))
//						terrain->SetHeightMap(resource);
//
//				}
//				catch (const std::bad_variant_access& error)
//				{
//					LOG_ERROR_F("%s", error.what());
//				}
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
	if (!animator)
		return;

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
					//�ִϸ��̼� �߰�
					if (auto payload = DragDropEvent::ReceiveDragDropPayload(PayloadType::Model))
					{
						actor_model->LoadFromFile(std::get<const char*>(payload->data));
					}
				}
			}
		}
		ShowAddAnimationButton();
	}
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
	if (!script)
		return;

	if (Inspector_Data::ComponentBegin(script->GetScriptName(), IconType::Component_Script, ComponentType::Script, script, true, true))
	{
		auto script_name = script->GetScriptName();

		ImGui::TextUnformatted("Script");
		ImGui::SameLine(70.0f);
		ImGui::PushID("##ScriptName");
		ImGui::PushItemWidth(200.0f);
		{
			ImGui::InputText("", &script_name, ImGuiInputTextFlags_ReadOnly);

			if (auto payload = DragDropEvent::ReceiveDragDropPayload(PayloadType::Script))
				script->SetScript(std::get<const char*>(payload->data));

			ImGui::SameLine();
			if (ImGui::Button("Edit"))
			{
				ScriptEditor::Get().SetScript(script->GetScriptPath());
				ScriptEditor::Get().SetVisible(true);
			}
		}
		ImGui::PopItemWidth();
		ImGui::PopID();
	}
	Inspector_Data::ComponentEnd();
}

void Widget_Inspector::ShowAudioSource(std::shared_ptr<class AudioSource>& audio_source)
{
	if (!audio_source)
		return;

	if (Inspector_Data::ComponentBegin("AudioSource", IconType::Component_AudioSource, ComponentType::AudioSource, audio_source, true, true))
	{
		auto audio_clip_name = audio_source->GetAudioClipName();
		auto is_playing = audio_source->IsPlaying();
		auto is_loop = audio_source->IsLoop();
		auto is_mute = audio_source->IsMute();
		auto priority = audio_source->GetPriority();
		auto volume = audio_source->GetVolume();
		auto pitch = audio_source->GetPitch();
		auto pan = audio_source->GetPan();

		//Audio Clip
		ImGui::TextUnformatted("Audio Clip");
		ImGui::SameLine(140.0f);
		ImGui::PushItemWidth(250.0f);
		ImGui::InputText("##AudioClipName", &audio_clip_name, ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();

		if (auto payload = DragDropEvent::ReceiveDragDropPayload(PayloadType::Audio))
			audio_source->SetAudioClip(std::get<const char*>(payload->data));

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

		if (is_playing != audio_source->IsPlaying())   audio_source->SetPlaying(is_playing);
		if (is_loop != audio_source->IsLoop())      audio_source->SetLoop(is_loop);
		if (is_mute != audio_source->IsMute())      audio_source->SetMute(is_mute);
		if (priority != audio_source->GetPriority()) audio_source->SetPriority(priority);
		if (volume != audio_source->GetVolume())   audio_source->SetVolume(volume);
		if (pitch != audio_source->GetPitch())    audio_source->SetPitch(pitch);
		if (pan != audio_source->GetPan())      audio_source->SetPan(pan);
	}
	Inspector_Data::ComponentEnd();
}

void Widget_Inspector::ShowAudioListener(std::shared_ptr<class AudioListener>& audio_listener)
{
	if (!audio_listener)
		return;

	if (Inspector_Data::ComponentBegin("AudioListener", IconType::Component_AudioListener, ComponentType::AudioListener, audio_listener, true, true))
	{

	}
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
				if (ImGui::MenuItem("Directional"))
					actor->AddComponent<Light>()->SetLightType(LightType::Directional);
				if (ImGui::MenuItem("Point"))
					actor->AddComponent<Light>()->SetLightType(LightType::Point);
				if (ImGui::MenuItem("Spot"))
					actor->AddComponent<Light>()->SetLightType(LightType::Spot);
				ImGui::EndMenu();
			}

			//Animation
			if (ImGui::MenuItem("Animation"))
				actor->AddComponent<Animator>();

			//Script
			if (ImGui::MenuItem("Script"))
				actor->AddComponent<Script>();

			//Audio
			if (ImGui::BeginMenu("Audio"))
			{
				if (ImGui::MenuItem("AudioSource"))
					actor->AddComponent<AudioSource>();
				if (ImGui::MenuItem("AudioListener"))
					actor->AddComponent<AudioListener>();
				ImGui::EndMenu();
			}
		}
		ImGui::EndPopup();
	}
}
