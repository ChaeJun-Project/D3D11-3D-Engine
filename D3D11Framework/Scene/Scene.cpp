#include "D3D11Framework.h"
#include "Scene.h"
#include "Actor.h"
#include "Component/Transform.h"
#include "Component/Camera.h"
#include "Component/Renderable.h"
#include "Component/Light.h"
#include "Component/SkyBox.h"

Scene::Scene(Context * context)
	: context(context)
{
   name = "New Scene";

   SUBSCRIBE_TO_EVENT(EventType::Scene_Refresh, [this]() {is_added=true;});
   
   renderer = context->GetSubsystem<Renderer>();
   
   CreateCamera("MainCamera");
}

Scene::~Scene()
{
	actors.clear();
	actors.shrink_to_fit();
}

auto Scene::GetRootActors() const -> const std::vector<std::shared_ptr<class Actor>>
{
    std::vector<std::shared_ptr<Actor>> root_actors;
	for (const auto& actor : actors)
	{
		if(actor->GetTransform()->IsRoot())
		   root_actors.emplace_back(actor);
	}

	return root_actors;
}

auto Scene::CreateActor(const bool& is_active) -> std::shared_ptr<class Actor>
{
	auto actor = std::make_shared<Actor>(context);
	actor->SetActive(is_active);
	actor->Initialize(actor->AddComponent<Transform>());
	AddActor(actor);

	return actor;
}

void Scene::AddActor(const std::shared_ptr<class Actor>& actor)
{
	actors.emplace_back(actor);
	is_added = true;
}

void Scene::RemoveActor(const std::shared_ptr<class Actor>& actor)
{
}

void Scene::Update()
{
   auto is_started=Engine::IsFlagEnabled(EngineFlags_Game) && is_editor_mode;
   auto is_stopped=!Engine::IsFlagEnabled(EngineFlags_Game) && !is_editor_mode;
   is_editor_mode=!Engine::IsFlagEnabled(EngineFlags_Game);

   if (is_started)
   {
	   for (const auto& actor : actors)
	   {
		   actor->Start();
	   }
   }

   if (is_stopped)
   {
	   for (const auto& actor : actors)
	   {
		   actor->Stop();
	   }
   }

   for (const auto& actor : actors)
   {
	   actor->Update();
   }

   if (is_added)
   {
	   renderer->AcquireRenderables(this);
	   is_added=false;
   }
}

auto Scene::CreateCamera(const std::string& camera_name) -> std::shared_ptr<class Actor>
{
	auto camera= CreateActor();
	camera->SetName(camera_name);
	camera->GetTransform()->SetTranslation(Vector3(0.0f,0.0f,-5.0f));
	camera->AddComponent<Camera>();

	return camera;
} 

auto Scene::CreateDirectionalLight() -> std::shared_ptr<class Actor>
{
	auto light = CreateActor();
	light->SetName("Directional Light");
	light->GetTransform()->SetRotation(Quaternion::QuaternionFromEulerAngle(Vector3(30.0f, 0.0f, 0.0f)));
	light->GetTransform()->SetTranslation(Vector3(0.0f, 10.0f, -5.0f));

	auto component = light->AddComponent<Light>();
	component->SetLightType(LightType::Directional);
	component->SetIntensity(1.5f);

	return light;
}

auto Scene::CreatePointLight() -> std::shared_ptr<class Actor>
{
	auto light = CreateActor();
	light->SetName("Point Light");

	auto component = light->AddComponent<Light>();
	component->SetLightType(LightType::Point);

	return light;
}

auto Scene::CreateSpotLight() -> std::shared_ptr<class Actor>
{
	auto light = CreateActor();
	light->SetName("Spot Light");
	
	auto component = light->AddComponent<Light>();
	component->SetLightType(LightType::Spot);

	return light;
}

auto Scene::CreateSkyBox() -> std::shared_ptr<class Actor>
{
	auto sky_box = CreateActor();
	sky_box->SetName("SkyBox");

	auto component = sky_box->AddComponent<SkyBox>();
	component->SetStandardSkyBox();

	return sky_box;
}

