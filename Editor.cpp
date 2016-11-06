#include "Application.h"
#include "Editor.h"
#include "Imgui\imgui.h"
#include "Random.h"
#include "FPSGraph.h"
#include "WindowOptions.h"
#include "HardwareInfo.h"
#include "Console.h"
#include "Assets.h"
#include "Profiler.h"
#include "DebugDraw.h"
#include "CameraWindow.h"

Editor::Editor(const char* name, bool start_enabled) : Module(name, start_enabled)
{
	windows.push_back(console = new Console()); //Create console in the constructor to get ALL init logs from other modules.
}

Editor::~Editor()
{}

bool Editor::Init(Data & config)
{
	return true;
}

bool Editor::Start()
{
	bool ret = true;

	LOG("Start Editor");

	//Create Windows
	windows.push_back(&g_Profiler);
	windows.push_back(fps_graph_win = new FPSGraph());
	windows.push_back(winoptions_win = new WindowOptions());
	windows.push_back(hardware_win = new HardwareInfo());
	windows.push_back(assets = new Assets());
	windows.push_back(camera_win = new CameraWindow());
	

	return ret;
}

bool Editor::CleanUp()
{
	LOG("Clean Up Editor");

	return true;
}

update_status Editor::Update(float dt)
{
	PROFILE("Editor::Update()");

	update_status ret = UPDATE_CONTINUE;

	ret = EditorWindows(); //Update the windows of the editor
	
	//Draw Grid
	Plane_P grid(0,1,0,0);
	grid.axis = true;
	grid.Render();

	//Shortcut to save. TODO: Do a better implementation of the shortcuts
	if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->go_manager->SaveScene();

	//Handle Quit event
	bool quit = false;
	if (App->input->Quit())
		quit = QuitWindow();

	if (quit)
		ret = UPDATE_STOP;

	return ret;	
}

update_status Editor::EditorWindows()
{
	update_status ret = UPDATE_CONTINUE;

	//Main MenuBar --------------------------------------------------------------------------------------------
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			FileMenu();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			EditMenu();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Windows"))
		{
			WindowsMenu();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			HelpMenu();
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Quit", NULL))
		{
			ret = UPDATE_STOP;
		}
		ImGui::EndMainMenuBar();
	}

	//Windows ----------------------------------------------------------------------------------------------------
	vector<Window*>::iterator win = windows.begin();
	while (win != windows.end())
	{
		PROFILE("Editor::Update-PaintWindows");
		(*win)->Draw();
		++win;
	}

	
	return ret;
}


void Editor::FileMenu()
{
	if (ImGui::MenuItem("Create New Scene"))
	{
		App->go_manager->LoadEmptyScene();
	}
}

//Menus -----------------------------------------------------------------------------------------------
void Editor::HelpMenu()
{
	if (ImGui::MenuItem("Documentation"))
		App->OpenURL("https://github.com/traguill/Ezwix-Engine/wiki");

	if (ImGui::MenuItem("Download latest build"))
		App->OpenURL("https://github.com/traguill/Ezwix-Engine/releases");

	if (ImGui::MenuItem("Report a bug"))
		App->OpenURL("https://github.com/traguill/Ezwix-Engine/issues");

	if (ImGui::MenuItem("About"))
	{
		ImGui::OpenPopup("##1 About");	
		if (ImGui::BeginPopup("##1 About"))
		{
			ImGui::Text("Ezwix Engine");
			ImGui::Text("A 3D engine for learning purposes.");
			ImGui::Text("Autor: Guillem Travila");
			ImGui::Text("Libraries: SDL 2.0, ImGui, MathGeoLib, Bullet, OpenGl 3.2");
			ImGui::Text("License"); //TODO

			ImGui::EndPopup();
		}
	}
}

void Editor::WindowsMenu()
{
	if (ImGui::BeginMenu("Configuration"))
	{
		if (ImGui::MenuItem("FPS Graph"))
		{
			fps_graph_win->SetActive(true);
		}
		if (ImGui::MenuItem("Window Options"))
		{
			winoptions_win->SetActive(true);
		}
		if (ImGui::MenuItem("Hardware Info"))
		{
			hardware_win->SetActive(true);
		}
			
		ImGui::EndMenu();
	}

	if (ImGui::MenuItem("Console"))
	{
		console->SetActive(true);
	}
	
	if (ImGui::MenuItem("Profiler"))
	{
		g_Profiler.SetActive(true);
	}

	if (ImGui::MenuItem("Assets"))
	{
		assets->SetActive(true);
	}
}

void Editor::EditMenu()
{
	if (ImGui::MenuItem("Camera"))
	{
		camera_win->SetActive(true);
	}
}

bool Editor::QuitWindow() const
{
	bool ret = false;

	//Show confirmation window
	const SDL_MessageBoxButtonData buttons[] = {
		{ SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "Save" },
		{ /*input*/								0,1, "Don't Save" },
		{										0, 2, "Cancel" },
	};
	const SDL_MessageBoxColorScheme colorScheme = 
	{
		{ 
		  /* [SDL_MESSAGEBOX_COLOR_BACKGROUND] */
			{ 255,   0,   0 },
			/* [SDL_MESSAGEBOX_COLOR_TEXT] */
			{ 0, 255,   0 },
			/* [SDL_MESSAGEBOX_COLOR_BUTTON_BORDER] */
			{ 255, 255,   0 },
			/* [SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND] */
			{ 0,   0, 255 },
			/* [SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED] */
			{ 255,   0, 255 }
		}
	};
	const SDL_MessageBoxData messageboxdata = {
		SDL_MESSAGEBOX_INFORMATION, /* .flags */
		App->window->window, /* .window */
		"The Scene has been modified", /* .title */
		"Do you want to save the changes you made in the scene? \n Your changes will be lost if you don't save them.", /* .message */
		SDL_arraysize(buttons), /* .numbuttons */
		buttons, /* .buttons */
		&colorScheme /* .colorScheme */
	};
	int buttonid;

	if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) 
	{
		LOG("Error displaying Quit Message Box");
	}
	
	switch (buttonid)
	{
	case 0: //Save
		App->go_manager->SaveScene();
		ret = true;
		break;
	case 1: //Quit
		ret = true;
		break;
	case 2: //Cancel
		App->input->ResetQuit();
		break;
	}

	return ret;
}
