#include "BlinkSystem.h"
#include "../Components/InventoryComponent.h"
#include "../Components/BodyComponent.h"
#include "../Components/CellComponent.h"
#include "../Components/DirectionComponent.h"
#include "../Components/BlinkComponent.h"
#include "../Components/LayerComponent.h"
#include "../Components/InputComponent.h"
#include "../EntityFactory.h"
#include "../PhysixSystem.h"
#include "../GameGlobals.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Events/SkillEvent.h"
#include "../Components/BlinkComponent.h"
#include "../Animation/Animator.h"
#include "../Animation/States/WalkingState.h"


BlinkSystem::BlinkSystem(LayerManager* layerManager)
{
	m_layerManager = layerManager;
}

BlinkSystem::~BlinkSystem()
{
}

void BlinkSystem::configure(entityx::EventManager &eventManager)
{
	eventManager.subscribe<SkillEvent>(*this);
}

void BlinkSystem::update(EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt)
{
	for (auto entity : entityManager.entities_with_components<BlinkComponent, InventoryComponent, CellComponent, DirectionComponent>())
	{
		auto body = entity.component<BodyComponent>();

		// Serverseitig
		if (body.valid())
		{
			auto layerComponent = entity.component<LayerComponent>();
			auto cell = entity.component<CellComponent>();
			auto direction = entity.component<DirectionComponent>();
            Entity blocks;
			switch (direction->direction)
			{
			case Direction::UP:
				blocks = m_layerManager->getEntityWithComponent<BodyComponent>(layerComponent->layer, cell->x, cell->y - 1);
				break;
			case Direction::DOWN:
				blocks = m_layerManager->getEntityWithComponent<BodyComponent>(layerComponent->layer, cell->x, cell->y + 1);
				break;
			case Direction::LEFT:
				blocks = m_layerManager->getEntityWithComponent<BodyComponent>(layerComponent->layer, cell->x - 1, cell->y);
				break;
			case Direction::RIGHT:
				blocks = m_layerManager->getEntityWithComponent<BodyComponent>(layerComponent->layer, cell->x + 1, cell->y);
				break;
			default: break;
			}
			if (blocks)
			{
				body->body->SetTransform(b2Vec2(PhysixSystem::toBox2D((cell->x)*GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH / 2.f), PhysixSystem::toBox2D((cell->y)*GameConstants::CELL_HEIGHT + GameConstants::CELL_HEIGHT / 2.f)), true);
				eventManager.emit<SkillEvent>(entity, SkillType::BLINK, false);
				return; // do not spawn further images
			}
			else
			{
				setVelocity(body->body, direction->direction);
			}
		}
		spawnAfterImages(entity, dt);
	}
}

void BlinkSystem::setVelocity(b2Body* body, Direction direction)
{
	float power = GameConstants::BLINK_SPEED;
	switch (direction)
	{
	case Direction::UP:
		body->SetLinearVelocity(b2Vec2(0, -power));
		break;
	case Direction::DOWN:
		body->SetLinearVelocity(b2Vec2(0, +power));
		break;
	case Direction::LEFT:
		body->SetLinearVelocity(b2Vec2(-power, 0));
		break;
	case Direction::RIGHT:
		body->SetLinearVelocity(b2Vec2(+power, 0));
		break;
	default: break;
	}
}

void BlinkSystem::spawnAfterImages(entityx::Entity &entity, entityx::TimeDelta dt)
{
	auto blink = entity.component<BlinkComponent>();
	auto cell = entity.component<CellComponent>();
	auto transform = entity.component<TransformComponent>();
	blink->afterImageCounter += static_cast<float>(dt);
	if (entity.has_component<SpriteComponent>() && blink->afterImageCounter >= 0.03f)
	{
		blink->afterImageCounter = 0;
		GameGlobals::entityFactory->createAfterimage(cell->x, cell->y, transform->x, transform->y, entity.component<SpriteComponent>()->sprite, 1.f);
		if (entity.has_component<AnimationComponent>())
		{
			auto animationComponent = entity.component<AnimationComponent>();
			animationComponent->animator->changeTo<WalkingState>(entity);
		}
	}
}

void BlinkSystem::receive(const SkillEvent& evt)
{
	if (evt.type == SkillType::BLINK)
	{
		Entity entity = evt.triggerEntity;
		auto body = entity.component<BodyComponent>();
		if (evt.activate)
		{
			entity.assign<BlinkComponent>();

			// Serverseitig
			if (body.valid())
			{
				entity.component<InputComponent>()->disabled = true;
				body->body->SetType(b2_dynamicBody);
				body->body->SetBullet(true);
			}
		}
		else
		{
			entity.remove<BlinkComponent>();

			// Serverseitig
			if (body.valid())
			{
				entity.component<InputComponent>()->disabled = false;
				body->body->SetBullet(false);
				body->body->SetType(b2_dynamicBody);
				body->body->SetLinearVelocity(b2Vec2_zero);
			}
		}
	}
}
