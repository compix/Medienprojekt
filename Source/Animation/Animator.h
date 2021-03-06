#pragma once
#include <unordered_map>
#include <memory>
#include "../Utils/make_unique.h"
#include <assert.h>
#include "entityx/entityx.h"
#include <typeinfo>
#include <typeindex>
#include "../Utils/AssetManagement/Assets.h"
#include "../Utils/Common.h"

enum class AnimationType : uint8_t;
class AnimationState;

class AnimationInfo
{
	friend class Animator;
public:
	AnimationInfo(const Assets::Animation* anim, const Assets::Texture* tex) : m_animation(anim), m_texture(tex) {}
	AnimationInfo() : m_animation(nullptr), m_texture(nullptr) {}
private:
	const Assets::Animation* m_animation;
	const Assets::Texture* m_texture;
};

class Animator
{
	friend class AnimatorManager;
public:
	void updateAnimation(AnimationType animationType, entityx::Entity& entity);

	template<class T>
	void changeTo(entityx::Entity& entity);

	void update(entityx::Entity& entity, float deltaTime);

private:
	void add(AnimationType animationType, const std::string& animationName, const std::string& textureName);
	const AnimationInfo& get(AnimationType animationType);	

	template<class T>
	AnimationState* add();
private:
	std::unordered_map<std::type_index, std::unique_ptr<AnimationState>> m_states;
	std::unordered_map<AnimationType, AnimationInfo, EnumClassHash> m_animations;
};

template <class T>
void Animator::changeTo(entityx::Entity& entity)
{
	if (m_states.count(typeid(T)) == 0)
		return;

	auto animationComponent = entity.component<AnimationComponent>();
	animationComponent->state = m_states[typeid(T)].get();
}

template <class T>
AnimationState* Animator::add()
{
	assert(m_states.count(typeid(T)) == 0);
	m_states[typeid(T)] = std::make_unique<T>();
	
	return m_states[typeid(T)].get();
}