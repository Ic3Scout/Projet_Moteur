#include "SceneManager.h"
#include "Scene.h"

void SceneManager::ChangeScene(std::string name)
{
	m_nextSceneName = name;
}

void SceneManager::Update(float dt)
{
	if (!m_nextSceneName.empty())
	{
		LoadScene();
		m_nextSceneName.clear();
	}
	if (m_currentScene)
		m_currentScene->Update(dt);
}

void SceneManager::LoadScene()
{
	auto it = m_scenes.find(m_nextSceneName);
	if (it == m_scenes.end())
		return;

	if (m_currentScene)
		m_currentScene->OnExit();

	m_currentScene = it->second;
	m_currentScene->OnEnter();

	m_nextSceneName.clear();
}
