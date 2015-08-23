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
		auto transform = entity.component<TransformComponent>();
		auto cell = entity.component<CellComponent>();
		auto direction = entity.component<DirectionComponent>();
		auto layerComponent = entity.component<LayerComponent>();

		Entity blocks;
		int x = 0, y = 0;
		switch (direction->direction)
		{
		case Direction::UP:
			blocks = m_layerManager->getEntityWithComponent<BodyComponent>(layerComponent->layer, cell->x, cell->y - 1);
			y = -1;
			break;
		case Direction::DOWN: 
			blocks = m_layerManager->getEntityWithComponent<BodyComponent>(layerComponent->layer, cell->x, cell->y + 1);
			y = 1;
			break;
		case Direction::LEFT:
			blocks = m_layerManager->getEntityWithComponent<BodyComponent>(layerComponent->layer, cell->x - 1, cell->y);
			x = -1;
			break;
		case Direction::RIGHT:
			blocks = m_layerManager->getEntityWithComponent<BodyComponent>(layerComponent->layer, cell->x + 1, cell->y);
			x = 1;
			break;
		default: break;
		}

		if (blocks)
		{
			auto entites = m_layerManager->getEntities(layerComponent->layer, cell->x, cell->y);
			bool blocked = false;
			for (auto it = entites.begin(); it != entites.end(); ++it)
			{
				if (it->id() != entity.id() && it->has_component<BodyComponent>()){
					blocked = true;
					break;
				}
			}

			if (blocked)
			{
				body->body->SetTransform(b2Vec2(PhysixSystem::toBox2D((cell->x - x)*GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH / 2.f), PhysixSystem::toBox2D((cell->y - y)*GameConstants::CELL_HEIGHT + GameConstants::CELL_HEIGHT / 2.f)), true);
			}
			body->body->SetBullet(false);
			body->body->SetType(b2_dynamicBody);
			body->body->SetLinearVelocity(b2Vec2_zero);
			entity.remove<BlinkComponent>();
			if (!entity.has_component<InputComponent>())
			{
				entity.assign<InputComponent>();
			}

			body->body->SetTransform(b2Vec2(PhysixSystem::toBox2D((cell->x)*GameConstants::CELL_WIDTH + GameConstants::CELL_WIDTH / 2.f), PhysixSystem::toBox2D((cell->y)*GameConstants::CELL_HEIGHT + GameConstants::CELL_HEIGHT / 2.f)), true);
		}
		else
		{
			if (entity.has_component<InputComponent>())
			{
				entity.remove<InputComponent>();
			}

			float power = GameConstants::BLINK_SPEED;
			body->body->SetType(b2_dynamicBody);
			body->body->SetBullet(true);
			b2Vec2 choosenPower = b2Vec2_zero;
			switch (direction->direction)
			{
			case Direction::UP:
				body->body->SetLinearVelocity(b2Vec2(0, -power));
				choosenPower = b2Vec2(0, -power);
				break;
			case Direction::DOWN:
				body->body->SetLinearVelocity(b2Vec2(0, +power));
				choosenPower = b2Vec2(0, +power);
				break;
			case Direction::LEFT:
				body->body->SetLinearVelocity(b2Vec2(-power, 0));
				choosenPower = b2Vec2(-power, 0);
				break;
			case Direction::RIGHT:
				body->body->SetLinearVelocity(b2Vec2(+power, 0));
				choosenPower = b2Vec2(+power, 0);
				break;
			default: break;
			}
			if (entity.has_component<SpriteComponent>())
			{
				GameGlobals::entityFactory->createAfterimage(cell->x, cell->y, transform->x, transform->y, entity.component<SpriteComponent>()->sprite, 1.f);
			}
			
		}
	}
}