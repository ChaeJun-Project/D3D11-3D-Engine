#include "D3D11Framework.h"
#include "Model.h"
#include "Scene/Actor.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/Renderable.h"
#include "Scene/Component/Animator.h"
#include "Importer/ModelImporter.h"

Model::Model(Context * context)
	:IResource(context, ResourceType::Model)
{
}

Model::~Model()
{
	meshes.clear();
	meshes.shrink_to_fit();

	materials.clear();
	materials.shrink_to_fit();

	animations.clear();
}

const bool Model::SaveToFile(const std::string & path)
{
	return false;
}

const bool Model::LoadFromFile(const std::string & path)
{
	auto resource_manager = context->GetSubsystem<ResourceManager>();

	//Model의 Mesh데이터인 경우
	if ((FileSystem::GetFileDataNameFromPath(path) == "Mesh"))
	{

		SetWorkingDirectories(resource_manager->GetProjectDirectory() + FileSystem::GetIntactFileNameFromPath(path));
		SetResourcePath(model_directory + FileSystem::GetIntactFileNameFromPath(path) + MODEL_BIN_EXTENSION);
		SetResourceName(FileSystem::GetIntactFileNameFromPath(path));

		//애니메이션을 제외한 모델의 모든 데이터를 추출
		if (resource_manager->GetModelImporter()->Load(this, path))
		{
			normalize_scale = ComputeNormalizedScale();
			root_actor->GetTransform()->SetScale(normalize_scale);
			root_actor->GetTransform()->UpdateTransform();
		}
		return true;
	}

	//Model의 Animation데이터인 경우
	else
	{
		//현재 추가된 애니메이션이 기존 애니메이션 목록에 없는 경우
		if (!FindAnimation(FileSystem::GetIntactFileNameFromPath(path)))
		{
			//애니메이션 데이터 추출
			if (resource_manager->GetModelImporter()->LoadAnimation(this, path))
			{
				//Widget_Inspector에서 actor의 Animator를 추가했으므로 받아올 수 있음
				auto animator = root_actor->GetComponent<Animator>();
				//애니메이터에 애니메이션 저장
				animator->SetAnimations(animations);
				//애니메이터에 Bone 데이터 저장
				animator->SetSkeleton(skeleton);
			}
		}
		return true;
	}

	return false;
}

auto Model::GetMaterial(const uint & index) -> std::shared_ptr<Material>
{
	if (index >= materials.size())
	{
		LOG_ERROR("Invalid parameter, out of range");
		return nullptr;
	}

	return materials[index];
}

void Model::AddMaterial(const std::shared_ptr<Material>& material, const std::shared_ptr<class Renderable>& renderable)
{
	if (!material || !renderable)
	{
		LOG_ERROR("Invalid parameter");
		return;
	}

	material->SetResourcePath(material_directory + material->GetResourceName() + MATERIAL_BIN_EXTENSION);
	material->SaveToFile(material->GetResourcePath());
	materials.emplace_back(material);

	renderable->SetMaterial(material);
}

void Model::AddTexture(const std::shared_ptr<Material>& material, const TextureType & texture_type, const std::string & path)
{
	if (!material || path == NOT_ASSIGNED_STR)
	{
		LOG_ERROR("Invalid parameter");
		return;
	}

	const auto texture_name = FileSystem::GetIntactFileNameFromPath(path);
	auto texture = context->GetSubsystem<ResourceManager>()->GetResourceFromName<Texture2D>(texture_name);

	if (texture)
		material->SetTexture(texture_type, texture);
	else
	{
		texture = std::make_shared<Texture2D>(context);
		texture->LoadFromFile(path);
		texture->SetResourcePath(texture_directory + texture_name + TEXTURE_BIN_EXTENSION);
		texture->SetResourceName(texture_name);
		texture->SaveToFile(texture->GetResourcePath());
		texture->ClearMipChain();

		material->SetTexture(texture_type, texture);
	}
}

auto Model::GetMesh(const uint & index) -> std::shared_ptr<Mesh>
{
	if (index >= meshes.size())
	{
		LOG_ERROR("Invalid parameter, out of range");
		return nullptr;
	}

	return meshes[index];
}

void Model::AddMesh(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<class Renderable>& renderable)
{
	if (!mesh || !renderable)
	{
		LOG_ERROR("Invalid parameter");
		return;
	}

	bound_box = BoundBox(mesh->GetVertices());

	mesh->SetResourcePath(mesh_directory + mesh->GetResourceName() + MESH_BIN_EXTENSION);
	mesh->SaveToFile(mesh->GetResourcePath());
	meshes.emplace_back(mesh);

	renderable->SetMesh(mesh);
	renderable->SetBoundBox(bound_box);
}

void Model::AddMesh(const std::vector<VertexTextureNormalTangentBlend>& vertices, const std::vector<uint>& indices, const std::shared_ptr<class Renderable>& renderable)
{
	if (vertices.empty() || indices.empty() || !renderable)
	{
		LOG_ERROR("Invalid parameter");
		return;
	}

	auto mesh = std::make_shared<Mesh>(context);
	mesh->SetVertices(vertices);
	mesh->SetIndices(indices);
	mesh->CreateBuffers();
	AddMesh(mesh, renderable);
}

void Model::AddBone(const std::string & name, const int & parent_index, const Matrix & offset)
{
	if (!skeleton)
		skeleton = std::make_shared<Skeleton>(context);

	skeleton->AddBone(name, parent_index, offset);
}

void Model::AddBone(const Bone & bone)
{
	if (!skeleton)
		skeleton = std::make_shared<Skeleton>(context);

	skeleton->AddBone(bone);
}

auto Model::FindBone(const uint & bone_index) const -> Bone *
{
	if (!skeleton)
	{
		LOG_ERROR("Skeleton is nullptr");
		return nullptr;
	}

	return skeleton->FindBone(bone_index);
}

auto Model::FindBone(const std::string & name) const -> Bone *
{
	if (!skeleton)
	{
		LOG_ERROR("Skeleton is nullptr");
		return nullptr;
	}

	return skeleton->FindBone(name);
}

auto Model::FindBoneIndex(const std::string & name) const -> const int
{
	if (!skeleton)
	{
		LOG_ERROR("Skeleton is nullptr");
		return -1;
	}

	return skeleton->FindBoneIndex(name);
}

void Model::AddAnimation(const std::string & name, const std::shared_ptr<Animation>& animation)
{
	if (animations.find(name) != animations.end())
	{
		LOG_ERROR("animation name is overlap");
		return;
	}

	animation->SetSkeleton(skeleton);
	animation->SortAnimationNodes(skeleton);

	animations[name] = animation;
}

auto Model::FindAnimation(const std::string & name) -> const std::shared_ptr<Animation>
{
	if (animations.find(name) == animations.end())
	{
		LOG_ERROR("animation is not found");
		return nullptr;
	}

	return animations[name];
}


void Model::SetWorkingDirectories(const std::string & directory)
{
	model_directory = directory + "/";
	material_directory = model_directory + "Material/";
	mesh_directory = model_directory + "Mesh/";
	texture_directory = model_directory + "Texture/";

	FileSystem::Create_Directory(model_directory);
	FileSystem::Create_Directory(material_directory);
	FileSystem::Create_Directory(mesh_directory);
	FileSystem::Create_Directory(texture_directory);
	FileSystem::Create_Directory(animation_directory);
}

auto Model::ComputeNormalizedScale() const -> const float
{
	const auto scale = bound_box.GetExtents().Length();

	return 1.0f / scale;
}
