#pragma once
#include <unordered_map>
#include <string>

class Scene;

class SceneManager
{
public:
	template<typename T>
	T* CreateScene(std::string name);

	void ChangeScene(std::string name);

	void Update(float dt);

	Scene* GetCurrentScene() const { return m_currentScene; }
private:
	void LoadScene();

	std::string m_nextSceneName;
	Scene* m_currentScene = nullptr;

	std::unordered_map<std::string, Scene*> m_scenes;

};

#include "SceneManager.inl"

