#include "BlinkSystem.h"
#include "../Components/InventoryComponent.h"
#include "../Components/BodyComponent.h"
#include "../Components/CellComponent.h"
#include "../Components/DirectionComponent.h"
#include "../Components/BlinkComponent.h"
#include "../Components/LayerComponent.h"
#include "../Components/InputComponent.h"


BlinkSystem::BlinkSystem(LayerManager* layerManager)
{
	m_layerManager = layerManager;
}

BlinkSystem::~BlinkSystem()
{
}

void BlinkSystem::configure(entityx::EventManager &event_manager)
{
	
}

void BlinkSystem::update(EntityManager &entityManager, entityx::EventManager &eventManager, entityx::TimeDelta dt)
{
	for (auto entity : entityManager.entities_with_components<BlinkComponent, InventoryComponent, CellComponent, BodyComponent, DirectionComponent>())
	{
		auto inventory = entity.component<InventoryComponent>();
		auto body = entity.component<BodyComponent>();
		auto cell = entity.component<CellComponent>();
		auto direction = entity.component<DirectionComponent>();
		auto layerComponent = entity.component<LayerComponent>();

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
			body->body->SetType(b2_dynamicBody);
			body->body->SetLinearVelocity(b2Vec2_zero);
			entity.remove<BlinkComponent>();
			if (!entity.has_component<InputComponent>())
			{
				entity.assign<InputComponent>();
			}
		} else
		{
			if (entity.has_component<InputComponent>())
			{
				entity.remove<InputComponent>();
			}

			float power = GameConstants::BLINK_SPEED;
			body->body->SetType(b2_kinematicBody);
			switch (direction->direction)
			{
			case Direction::UP:
				body->body->SetLinearVelocity(b2Vec2(0, -power));
				break;
			case Direction::DOWN:
				body->body->SetLinearVelocity(b2Vec2(0, +power));
				break;
			case Direction::LEFT:
				body->body->SetLinearVelocity(b2Vec2(-power, 0));
				break;
			case Direction::RIGHT:
				body->body->SetLinearVelocity(b2Vec2(+power, 0));
				break;
			default: break;
			}
		}
	}
}