#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"
#include "GameObject.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	go_cam = new GameObject();
	go_cam->name = "Editor Camera";
	cam_transform = (ComponentTransform*)go_cam->AddComponent(C_TRANSFORM);
	editor_cam = (ComponentCamera*)go_cam->AddComponent(C_CAMERA);

	current_camera = editor_cam;
}

ModuleCamera3D::~ModuleCamera3D()
{}

bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	return ret;
}

bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	delete go_cam; //Note: components are destroyed inside the destructor of the GameObject

	return true;
}

update_status ModuleCamera3D::Update(float dt)
{
	if(current_camera == editor_cam)
		EditorCameraMovement(dt);

	//If the current camera properties has been modified update the projection matrix.
	if (current_camera->properties_modified)
	{
		App->renderer3D->SetPerspective(current_camera->GetProjectionMatrix());
		current_camera->properties_modified = false;
	}

	return UPDATE_CONTINUE;
}

math::float3 ModuleCamera3D::GetPosition() const
{
	return current_camera->GetGameObject()->GetGlobalMatrix().TranslatePart();
}

math::float4x4 ModuleCamera3D::GetViewMatrix() const
{
	return current_camera->GetViewMatrix();
}

float ModuleCamera3D::GetNearPlane() const
{
	return current_camera->GetNearPlane();
}

float ModuleCamera3D::GetFarPlane() const
{
	return current_camera->GetFarPlane();
}

float ModuleCamera3D::GetFOV() const
{
	return current_camera->GetFOV();
}

void ModuleCamera3D::SetNearPlane(const float & near_plane)
{
	current_camera->SetNearPlane(near_plane);
}

void ModuleCamera3D::SetFarPlane(const float & far_plane)
{
	current_camera->SetFarPlane(far_plane);
}

void ModuleCamera3D::SetFOV(const float & fov)
{
	current_camera->SetFOV(fov);
}

void ModuleCamera3D::SetBackgroundColor(const math::float3 & color)
{
	current_camera->SetBackgroundColor(color);
	App->renderer3D->SetClearColor(color);
}

math::float3 ModuleCamera3D::GetBackgroundColor() const
{
	return current_camera->GetBackgroundColor();
}

ComponentCamera * ModuleCamera3D::GetCurrentCamera() const
{
	return current_camera;
}

void ModuleCamera3D::ChangeCurrentCamera(ComponentCamera * camera)
{
	if (camera != nullptr)
	{
		current_camera = camera;
		App->renderer3D->SetPerspective(current_camera->GetProjectionMatrix());
		App->renderer3D->SetClearColor(current_camera->GetBackgroundColor());
	}
}

ComponentCamera * ModuleCamera3D::GetEditorCamera() const
{
	return editor_cam;
}

void ModuleCamera3D::EditorCameraMovement(float dt)
{

	math::float3 position = cam_transform->GetPosition();
	float speed = 8.0f * dt;

	math::float4x4 matrix = cam_transform->GetGlobalMatrix();

	math::float3 world_x = matrix.WorldX();
	math::float3 world_y = matrix.WorldY();
	math::float3 world_z = matrix.WorldZ();

	//TODO: rotation doesn't apply correctly.
	math::float3 new_pos = math::float3::zero;

	//Arrows movement
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) new_pos += world_z * speed;
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) new_pos -= world_z * speed;
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) new_pos += world_x * speed;
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) new_pos -= world_x * speed;

	//Middle mouse button movement
	if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT)
	{
		int dx = App->input->GetMouseXMotion();
		int dy = App->input->GetMouseYMotion();

		new_pos += world_x * speed * dx;
		new_pos += world_y * speed * dy;
	}

	//Mouse wheel zoom
	float wheel_speed = 200 * dt;

	if (App->input->GetMouseZ() > 0) new_pos += world_z * speed * wheel_speed;
	if (App->input->GetMouseZ() < 0) new_pos -= world_z * speed * wheel_speed;


	if (new_pos.x != 0 || new_pos.y != 0 || new_pos.z != 0)
	{
		position += new_pos;
		cam_transform->SetPosition(position);
	}

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		math::Quat horizontal_rotation;
		horizontal_rotation.SetFromAxisAngle(world_y.Normalized(), DegToRad(dx * speed));
		cam_transform->Rotate(horizontal_rotation);

		cam_transform->Update(dt); //Needs to update to apply first rotation. Needs to get WorldX updated.

		math::Quat vertical_rotation;
		vertical_rotation.SetFromAxisAngle(cam_transform->GetGlobalMatrix().WorldX().Normalized(), DegToRad(dy * speed));
		cam_transform->Rotate(vertical_rotation);
	}

	//Update Transform component manually
	cam_transform->Update(dt);
}



