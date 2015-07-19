#pragma once
#include <unordered_map>
#include "Animator.h"
#include <typeinfo>

enum class AnimatorType
{
	PLAYER0,
	PLAYER1,
	PLAYER2,
	PLAYER3
};

class AnimatorManager
{
public:
	static void init();
	static void assignCharacterAnimator(entityx::Entity& entity, int playerIndex);
private:
	static void initCharacterAnimator(AnimatorType type, const std::string &textureName);
private:
	static std::unordered_map<AnimatorType, std::unique_ptr<Animator>, EnumClassHash> m_animators;
};