#include "LayerManager.h"
#include <iostream>
#include "Components/CellComponent.h"
#include "Components/TransformComponent.h"
#include "GameConstants.h"
#include "Components/SolidBlockComponent.h"
#include "Components/LayerComponent.h"
#include "GameGlobals.h"
#include "Components/DynamicComponent.h"
#include "AI/PathFinding/Graph.h"

EntityLayer* LayerManager::createLayer(uint8_t width, uint8_t height, int layer)
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
	}
}

/**
 * @brief	Refreshes cell positions of entities. (Transform - Cell synchronization)
 */
void LayerManager::update()
{
	for (auto entity : GameGlobals::entities->entities_with_components<DynamicComponent>())
	{
		updateCell(entity);
	}
}

EntityCollection LayerManager::getEntities(int layer, uint8_t cellX, uint8_t cellY)
{
	assert(m_layers.count(layer));
	return m_layers[layer]->get(cellX, cellY);
}

bool LayerManager::isInLayer(int layer, entityx::Entity& entity)
{
	assert(entity.has_component<CellComponent>());
	assert(m_layers.count(layer));
	auto cell = entity.component<CellComponent>();
	for (auto e : m_layers[layer]->get(cell->x, cell->y))
		if (e == entity)
			return true;

	return false;
}

bool LayerManager::isFree(int layer, uint8_t cellX, uint8_t cellY)
{
	return getEntities(layer, cellX, cellY).size() == 0;
}

void LayerManager::updateCell(entityx::Entity& entity)
{
	auto layerComponent = entity.component<LayerComponent>();
	auto cell = entity.component<CellComponent>();
	auto transform = entity.component<TransformComponent>();

	assert(layerComponent && cell && transform);
	assert(m_layers.count(layerComponent->layer));
	uint8_t cellX = static_cast<uint8_t>(transform->x / GameConstants::CELL_WIDTH);
	uint8_t cellY = static_cast<uint8_t>(transform->y / GameConstants::CELL_HEIGHT);
	
	if (!isInLayer(layerComponent->layer, entity))
	{
		// Layer of entity changed -> remove it from old layer
		for (auto& layerPair : m_layers)
			layerPair.second->remove(entity, cell->x, cell->y);

		m_layers[layerComponent->layer]->add(entity, cell->x, cell->y);
	}

	// If the cell changed then refresh the cell and grid
	if (cellX != cell->x || cellY != cell->y)
	{
		m_layers[layerComponent->layer]->remove(entity, cell->x, cell->y);

		cell->x = cellX;
		cell->y = cellY;

		m_layers[layerComponent->layer]->add(entity, cell->x, cell->y);
	}
}