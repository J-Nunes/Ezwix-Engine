#ifndef __COMPONENT_H__
#define __COMPONENT_H__

enum ComponentType
{
	C_TRANSFORM,
	C_MESH,
	C_MATERIAL,
	C_CAMERA
};

class GameObject;
class Data;

class Component
{
public:
	
	Component(ComponentType type, GameObject* game_object);
	virtual ~Component();

	virtual void Update(float dt);

	bool IsActive();
	void SetActive(bool value);
	virtual void OnInspector();

	ComponentType GetType()const;
	GameObject* GetGameObject()const;
	unsigned int GetUUID()const;

	virtual void OnTransformModified();
	virtual void Save(Data& file) const;
	virtual void Remove();

protected:
	bool active = true;
	ComponentType type;
	GameObject* game_object = nullptr;
	unsigned int uuid = 0;

};
#endif // !__COMPONENT_H__