#include "stdafx.h"
#include "Widget_Scene.h"
#include "ImGui/ImGuizmo.h"
#include "Scene/Scene.h"
#include "Scene/Actor.h"
#include "Scene/Component/Camera.h"
#include "Scene/Component/Transform.h"

Widget_Scene::Widget_Scene(Context * context)
	: IWidget(context)
{
	title = "Scene";
	window_flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse; //��ũ�� �ɼ� ����
	timer = context->GetSubsystem<Timer>();
	renderer = context->GetSubsystem<Renderer>();
}

void Widget_Scene::Render()
{
	ShowFrame();
	ShowTransformGizmo();
}

//�� �����Ӹ��� ����ڿ� ���� ����� Scene ������â�� ũ�⸣ �����ϰ� �������� �ؽ�ó�� ������â�� ���
void Widget_Scene::ShowFrame()
{
	//Scene ������â�� �׸� ��ġ�� �޾ƿ�
	auto frame_position_x = static_cast<uint>(ImGui::GetCursorPos().x + ImGui::GetWindowPos().x);
	auto frame_position_y = static_cast<uint>(ImGui::GetCursorPos().y + ImGui::GetWindowPos().y);

	//Scene ������â�� ũ�⸦ �޾ƿ�
	auto frame_width = static_cast<uint>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
	auto frame_height = static_cast<uint>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);

	//Scene ������â�� ũ�⸦ �׻� ¦���� ����
	frame_width -= frame_width % 2 != 0 ? 1 : 0;
	frame_height -= frame_height % 2 != 0 ? 1 : 0;

	//renderer���� Scene ������â�� �׸� ������ ����
	renderer->SetEditorOffset(Vector2(static_cast<float>(frame_position_x), static_cast<float>(frame_position_y)));

	//0.1�ʸ��� 
	if (frame_counter >= 0.1f)
	{
		renderer->SetResolution(frame_width, frame_height); //�������Ǵ� â ����
		frame_counter = 0.0f;
	}
	frame_counter += timer->GetDeltaTimeSec();

	//�������� ID3D11ShaderResourceView�� ImGui���� ������ �� �ֵ��� ���
	ImGui::Image
	(
		renderer->GetFrameResource(),
		ImVec2(static_cast<float>(frame_width), static_cast<float>(frame_height)),
		ImVec2(0, 0),
		ImVec2(1, 1),
		ImVec4(1, 1, 1, 1),
		ImVec4(1, 0, 0, 1)
	);

	//������ ������ ���콺 ���� ��ư�� ������ ���
	if (ImGui::IsMouseClicked(0) && ImGui::IsItemHovered())
		Editor_Helper::GetInstance()->Picking(); //�ش� Actor ��ŷ

	//Drag&Drop Event
	//Sceneâ�� fbx������ Drag&Drop�ϸ� Scene�� ���� ���
	if (auto payload = DragDropEvent::ReceiveDragDropPayload(PayloadType::Model))
		Editor_Helper::GetInstance()->LoadModel(std::get<const char*>(payload->data));
}

void Widget_Scene::ShowTransformGizmo()
{
	//���õ� Actor �����Ͱ� ����� ���(���� ī��Ʈ�� 0�� �Ǿ� ������ ���)
	if (Editor_Helper::GetInstance()->selected_actor.expired())
		return;

	auto camera = renderer->GetCamera();
	auto actor = Editor_Helper::GetInstance()->selected_actor.lock();

	//Scene�� ī�޶� �ְ� ���õ� actor�� ī�޶� ���۳�Ʈ�� ���� ���
	if (camera && !actor->GetComponent<Camera>())
	{
		auto transform = actor->GetTransform();

		if (!transform)
			return;

		static ImGuizmo::OPERATION operation(ImGuizmo::TRANSLATE);
		static ImGuizmo::MODE mode(ImGuizmo::WORLD);

		if (ImGui::IsKeyPressed(87)) //w
			operation = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(69)) //e
			operation = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(82)) //r
			operation = ImGuizmo::SCALE;

		auto offset = renderer->GetEditorOffset();
		auto size = renderer->GetResolution();
		//D3D�� ����� ����� ����ϰ� OpenGL�� ������ ����� ����ϴµ�
		//ImGuizmo�� OpenGLó�� ������ ����� ����ϱ� ������
		//���α׷� ������ ����ϴ� ��� ��Ʈ������ ��� ��ġ��ķ� �ٲ� �Ѱ���� ���ϴ� ���·� �������� ��
		auto view = camera->GetViewMatrix().Transpose();		//�� ��Ʈ������ ��ġ���
		auto proj = camera->GetProjectionMatrix().Transpose();	//���� ��Ʈ������ ��ġ���
		auto world = transform->GetWorldMatrix().Transpose();	//���� ��Ʈ������ ��ġ���

		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(offset.x, offset.y, size.x, size.y);
		ImGuizmo::Manipulate
		(
			view,
			proj,
			operation,
			mode,
			world
		);

		//���� ��Ʈ������ ��ġ��ĸ� �ٽ� ��ġ��ķ� ��ȯ -> ���α׷� ������ ����ϴ� ����� ��ķ� �ٽ� ��ȯ�Ѵٴ� �ǹ�
		world.Transpose();

		transform->SetTranslation(world.GetTranslation());
		transform->SetRotation(world.GetRotation());
		transform->SetScale(world.GetScale());
	}

	return;
}
