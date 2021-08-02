#include "stdafx.h"
#include "Scene/Actor.h"
#include "Scene/Component/Camera.h"

//싱글톤 인스턴스 초기화
//정적 멤버 변수이므로 객체 생성 전에 반드시 미리 초기화를 해야 해서
//생성자가 아닌 클래스 외부에서 초기화해야 함
Editor_Helper* Editor_Helper::instance = nullptr;

void Editor_Helper::Initialize(Context * context)
{
	this->context          = context;

	this->resource_manager = this->context->GetSubsystem<ResourceManager>();
	this->scene_manager    = this->context->GetSubsystem<SceneManager>();
	this->thread           = this->context->GetSubsystem<Thread>();
	this->renderer         = this->context->GetSubsystem<Renderer>();
	this->input            = this->context->GetSubsystem<Input>();
}

void Editor_Helper::LoadModel(const std::string & path)
{
  //Model Load 작업이 오래걸리는 작업이므로 여러 스레드가 분할 작업을 하여
  //성능향상을 할 수 있도록 지원
  thread->AddTask([this, path]() 
  {
      resource_manager->Load<Model>(path);
  });
}

void Editor_Helper::Picking()
{
    const auto& camera = renderer->GetCamera();

	if(!camera)
	  return;

    std::shared_ptr<Actor> actor;

	//현재 마우스의 위치를 통해 피킹된 오브젝트의 엑터를 받아옴
	camera->Pick(input->GetMousePosition(), actor);

	//Weak 포인터에 참조
	selected_actor = actor;
}
