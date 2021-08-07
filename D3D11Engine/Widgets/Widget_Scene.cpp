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
	window_flags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse; //스크롤 옵션 제거
	timer = context->GetSubsystem<Timer>();
	renderer = context->GetSubsystem<Renderer>();
}

void Widget_Scene::Render()
{
	ShowFrame();
	ShowTransformGizmo();
}

//매 프레임마다 사용자에 의해 변경된 Scene 윈도우창의 크기르 설정하고 렌더링된 텍스처를 윈도우창에 등록
void Widget_Scene::ShowFrame()
{
	//Scene 윈도우창을 그릴 위치를 받아옴
	auto frame_position_x = static_cast<uint>(ImGui::GetCursorPos().x + ImGui::GetWindowPos().x);
	auto frame_position_y = static_cast<uint>(ImGui::GetCursorPos().y + ImGui::GetWindowPos().y);

	//Scene 윈도우창의 크기를 받아옴
	auto frame_width = static_cast<uint>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
	auto frame_height = static_cast<uint>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);

	//Scene 윈도우창의 크기를 항상 짝수로 설정
	frame_width -= frame_width % 2 != 0 ? 1 : 0;
	frame_height -= frame_height % 2 != 0 ? 1 : 0;

	//renderer에서 Scene 윈도우창을 그릴 오프셋 설정
	renderer->SetEditorOffset(Vector2(static_cast<float>(frame_position_x), static_cast<float>(frame_position_y)));

	//0.1초마다 
	if (frame_counter >= 0.1f)
	{
		renderer->SetResolution(frame_width, frame_height); //렌더링되는 창 설정
		frame_counter = 0.0f;
	}
	frame_counter += timer->GetDeltaTimeSec();

	//렌더링된 ID3D11ShaderResourceView를 ImGui에서 보여줄 수 있도록 등록
	ImGui::Image
	(
		renderer->GetFrameResource(),
		ImVec2(static_cast<float>(frame_width), static_cast<float>(frame_height)),
		ImVec2(0, 0),
		ImVec2(1, 1),
		ImVec4(1, 1, 1, 1),
		ImVec4(1, 0, 0, 1)
	);

	//아이템 위에서 마우스 왼쪽 버튼을 눌렀을 경우
	if (ImGui::IsMouseClicked(0) && ImGui::IsItemHovered())
		Editor_Helper::GetInstance()->Picking(); //해당 Actor 피킹

	//Drag&Drop Event
	//Scene창에 fbx파일을 Drag&Drop하면 Scene에 모델을 띄움
	if (auto payload = DragDropEvent::ReceiveDragDropPayload(PayloadType::Model))
		Editor_Helper::GetInstance()->LoadModel(std::get<const char*>(payload->data));
}

void Widget_Scene::ShowTransformGizmo()
{
	//선택된 Actor 포인터가 만료된 경우(참조 카운트가 0이 되어 해제된 경우)
	if (Editor_Helper::GetInstance()->selected_actor.expired())
		return;

	auto camera = renderer->GetCamera();
	auto actor = Editor_Helper::GetInstance()->selected_actor.lock();

	//Scene에 카메라가 있고 선택된 actor에 카메라 컴퍼넌트가 없는 경우
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
		//D3D는 행기준 행렬을 사용하고 OpenGL은 열기준 행렬을 사용하는데
		//ImGuizmo는 OpenGL처럼 열기준 행렬을 사용하기 때문에
		//프로그램 내에서 사용하는 행렬 메트릭스를 모두 전치행렬로 바꿔 넘겨줘야 원하는 형태로 렌더링이 됨
		auto view = camera->GetViewMatrix().Transpose();		//뷰 메트릭스의 전치행렬
		auto proj = camera->GetProjectionMatrix().Transpose();	//투영 메트릭스의 전치행렬
		auto world = transform->GetWorldMatrix().Transpose();	//월드 메트릭스의 전치행렬

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

		//월드 메트릭스의 전치행렬를 다시 전치행렬로 변환 -> 프로그램 내에서 사용하는 행기준 행렬로 다시 변환한다는 의미
		world.Transpose();

		transform->SetTranslation(world.GetTranslation());
		transform->SetRotation(world.GetRotation());
		transform->SetScale(world.GetScale());
	}

	return;
}
