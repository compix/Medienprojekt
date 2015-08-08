#include "LayerManager.h"
#include <iostream>
#include "Components/CellComponent.h"
#include "Components/TransformComponent.h"
#include "GameConstants.h"
#include "Components/SolidBlockComponent.h"
#include "Components/LayerComponent.h"
#include "GameGlobals.h"
#include "Components/DynamicComponent.h"
#include "Utils/PathFinding/Graph.h"

EntityLayer* LayerManager::createLayer(int width, int height, int layer)
{
	m_layers[layer] = std::make_shared<EntityLayer>(width, height, layer);

	return m_layers[layer].get();
}

LayerManager::~LayerManager()
{
	GameGlobals::events->unsubscribe<entityx::EntityDestroyedEvent>(*this);
}

void LayerManager::reset()
{
	for (auto& kv : m_layers)
		kv.second->reset();
}

void LayerManager::configure(entityx::EventManager& events)
{
	events.subscribe<entityx::EntityDestroyedEvent>(*this);
}

void LayerManager::receive(const entityx::EntityDestroyedEvent& e)
{
	Entity entity = e.entity;
	auto cell = entity.component<CellComponent>();

	if (cell)
	{
		remove(entity);
	}
}

void LayerManager::add(Entity entity)
{
	auto cell = entity.component<CellComponent>();
	auto layerComponent = entity.component<LayerComponent>();

	if (!cell)
		std::cout << "Warning: Attempt to add an entity failed. Reason: " << entity << " does not have a CellComponent." << std::endl;

	if (!layerComponent)
		std::cout << "Warning: Attempt to add an entity failed. Reason: " << entity << " does not have a LayerComponent." << std::endl;

	if (cell && layerComponent)
	{
		if (m_layers.count(layerComponent->layer) == 0)
		{
			std::cout << "Warning: Attempt to add an entity failed. Reason: Layer " << layerComponent->layer << " does not exist." << std::endl;
			std::cout << "Please create the Layer first." << std::endl;
			return;
		}

		m_layers[layerComponent->layer]->add(entity, cell->x, cell->y);
		//m_scheduledForPush.push_back(EntityUpdateInfo(entity, cell->x, cell->y, layerComponent->layer));
	}
}

void LayerManager::remove(Entity entity)
{
	auto cell = entity.component<CellComponent>();
	auto layerComponent = entity.component<LayerComponent>();

	if (!cell)
		std::cout << "Warning: Attempt to remove an entity failed. Reason: " << entity << " does not have a CellComponent." << std::endl;

	if (!layerComponent)
		std::cout << "Warning: Attempt to remove an entity failed. Reason: " << entity << " does not have a LayerComponent." << std::endl;

	if (cell && layerComponent)
	{
		if (m_layers.count(layerComponent->layer) == 0)
		{
			std::cout << "Warning: Attempt to remove an entity failed. Reason: Layer " << layerComponent->layer << " does not exist." << std::endl;
			std::cout << "Please create the Layer first." << std::endl;
			return;
		}

		m_layers[layerComponent->layer]->remove(entity, cell->x, cell->y);
		//m_scheduledForRemoval.push_back(EntityUpdateInfo(entity, cell->x, cell->y, layerComponent->layer));
	}
		
}

/**
 * @brief	Refreshes cell positions of entities. (Transform - Cell synchronization)
 */
void LayerManager::update()
{
	/*
	for (auto& info : m_scheduledForPush)
		m_layers[info.layer]->add(info.entity, info.x, info.y);

	for (auto& info : m_scheduledForRemoval)
		m_layers[info.layer]->remove(info.entity, info.x, info.y);

	m_scheduledForPush.clear();
	m_scheduledForRemoval.clear();
	*/

	for (auto entity : GameGlobals::entities->entities_with_components<DynamicComponent>())
	{
		auto layerComponent = entity.component<LayerComponent>();
		auto cell = entity.component<CellComponent>();
		auto transform = entity.component<TransformComponent>();

		assert(layerComponent && cell && transform);

		if (cell && transform)
		{
			int cellX = static_cast<int>(transform->x / GameConstants::CELL_WIDTH);
			int cellY = static_cast<int>(transform->y / GameConstants::CELL_HEIGHT);

			// If the cell changed then refresh the cell and grid
			if (cellX != cell->x || cellY != cell->y)
			{
				m_layers[layerComponent->layer]->remove(entity, cell->x, cell->y);

				cell->x = cellX;
				cell->y = cellY;

				m_layers[layerComponent->layer]->add(entity, cell->x, cell->y);
			}
		}
	}
}

EntityCollection LayerManager::getEntities(int layer, int cellX, int cellY)
{
	assert(m_layers.count(layer));
	return m_layers[layer]->get(cellX, cellY);
}

bool LayerManager::isFree(int layer, int cellX, int cellY)
{
	return getEntities(layer, cellX, cellY).size() == 0;
}