#include "stdafx.h"
#include "Scene/Actor.h"
#include "Scene/Component/Camera.h"

//�̱��� �ν��Ͻ� �ʱ�ȭ
//���� ��� �����̹Ƿ� ��ü ���� ���� �ݵ�� �̸� �ʱ�ȭ�� �ؾ� �ؼ�
//�����ڰ� �ƴ� Ŭ���� �ܺο��� �ʱ�ȭ�ؾ� ��
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
  //Model Load �۾��� �����ɸ��� �۾��̹Ƿ� ���� �����尡 ���� �۾��� �Ͽ�
  //��������� �� �� �ֵ��� ����
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

	//���� ���콺�� ��ġ�� ���� ��ŷ�� ������Ʈ�� ���͸� �޾ƿ�
	camera->Pick(input->GetMousePosition(), actor);

	//Weak �����Ϳ� ����
	selected_actor = actor;
}
