template<typename T>
T* SceneManager::CreateScene(std::string name)
{
	if (m_scenes.find(name) != m_scenes.end())
	{
		return nullptr;
	}
	T* scene = new T();
	m_scenes[name] = scene;

	if(m_currentScene == nullptr)
	{
		m_currentScene = scene;
	}

	return scene;
}