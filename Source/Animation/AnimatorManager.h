#pragma once
#include <unordered_map>
#include "Animator.h"
#include <typeinfo>

enum class AnimatorType
{
	CHARACTER
};

class AnimatorManager
{
public:
	static void init();
	static void assignCharacterAnimator(entityx::Entity& entity);
private:
	static void initCharacterAnimator();
private:
	static std::unordered_map<AnimatorType, std::unique_ptr<Animator>> m_animators;
};