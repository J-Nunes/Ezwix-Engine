#ifndef __MODULE_H__
#define __MODULE_H__

#include "Data.h"
#include <string>

struct PhysBody3D;

#define NAME_MODULE_LENGTH 32

class Module
{
private :
	bool enabled;
	std::string name;
public:

	Module (const char* name, bool start_enabled = true) : name(name)
	{}

	virtual ~Module()
	{}

	virtual bool Init(Data& config) 
	{
		return true; 
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	virtual void OnCollision(PhysBody3D* body1, PhysBody3D* body2)
	{}

	const char* GetName()const
	{
		return name.data();
	}
};

#endif // !__MOUDLE_H__
