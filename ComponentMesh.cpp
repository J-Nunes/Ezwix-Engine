#include "ComponentMesh.h"
#include "Application.h"
#include "ModuleMeshes.h"
#include "imgui\imgui.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"

#include "ComponentCamera.h"

ComponentMesh::ComponentMesh(ComponentType type, GameObject* game_object) : Component(type, game_object)
{
	aabb.SetNegativeInfinity();
	bounding_box.SetNegativeInfinity();
}

ComponentMesh::~ComponentMesh()
{
	delete mesh;
	mesh = nullptr;
}

void ComponentMesh::Update(float dt)
{
	//Component must be active to update
	if (!IsActive())
		return;
	if (mesh)
	{
		GameObject* go = GetGameObject();
		ComponentMaterial* material = (ComponentMaterial*)GetGameObject()->GetComponent(C_MATERIAL);
		if (material)
			go->texture_to_draw = material->texture_id;
		go->mesh_to_draw = mesh;
		go->bounding_box = &bounding_box;

		App->renderer3D->AddToDraw(GetGameObject());
	}
}

void ComponentMesh::OnInspector()
{
	if (ImGui::CollapsingHeader("Geometry Mesh"))
	{
		//Active
		bool is_active = IsActive();
		if (ImGui::Checkbox("###activeMesh", &is_active))
		{
			SetActive(is_active);
		}

		if (mesh)
		{
			ImGui::Text("Number of vertices %d", mesh->num_vertices);
			ImGui::Text("Number of indices %d", mesh->num_indices);

			if (mesh->uvs != nullptr)
				ImGui::Text("Has UVs: yes");
			else
				ImGui::Text("Has UVs: no");

			if (mesh->normals != nullptr)
				ImGui::Text("Has Normals: yes");
			else
				ImGui::Text("Has Normals: no");

			if (mesh->colors != nullptr)
				ImGui::Text("Has Colors: yes");
			else
				ImGui::Text("Has Colors: no");
		}
		else
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "WARNING");
			ImGui::SameLine(); ImGui::Text("No mesh was loaded.");
		}

		if (ImGui::Button("###mesh_rem Remove"))
		{
			Remove();
		}
	}
}

void ComponentMesh::OnTransformModified()
{
	RecalculateBoundingBox();
}

bool ComponentMesh::SetMesh(Mesh * mesh)
{
	bool ret = false;
	if (mesh)
	{
		this->mesh = mesh;

		aabb.Enclose((float3*)mesh->vertices, mesh->num_vertices);
		RecalculateBoundingBox();
		ret = true;
	}
		
	return ret;
}

void ComponentMesh::RecalculateBoundingBox()
{
	ComponentTransform* trans = (ComponentTransform*)GetGameObject()->GetComponent(C_TRANSFORM);
	assert(trans);

	math::OBB ob = aabb.Transform(trans->GetGlobalMatrix());
	bounding_box = ob.MinimalEnclosingAABB();
}

void ComponentMesh::Save(Data & file)const
{
	Data data;
	data.AppendInt("type", type);
	data.AppendInt("UUID", uuid);
	data.AppendBool("active", active);
	data.AppendString("path", mesh->file_path.data());

	file.AppendArrayValue(data);
}
