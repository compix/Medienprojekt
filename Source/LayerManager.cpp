#include "LayerManager.h"
#include <iostream>
#include "Components/CellComponent.h"
#include "Components/TransformComponent.h"
#include "GameConstants.h"
#include "Components/SolidBlockComponent.h"
#include "Components/LayerComponent.h"

EntityLayer* LayerManager::createLayer(int width, int height, int layer)
{
	m_layers[layer] = std::make_shared<EntityLayer>(width, height, layer);

	return m_layers[layer].get();
}

void LayerManager::configure(entityx::EventManager& events)
{
	events.subscribe<EntityDestroyedEvent>(*this);
}

void LayerManager::receive(const EntityDestroyedEvent& e)
{
	Entity entity = e.entity;
	auto cell = entity.component<CellComponent>();

	if (cell)
	{
		remove(entity);
	}
}

void LayerManager::addToLayer(int layer, Entity entity)
{
	if (m_layers.count(layer))
	{
		auto cellPosition = entity.component<CellComponent>();
		if (cellPosition)
		{
			m_layers[layer]->add(entity, cellPosition->x, cellPosition->y);
			if (entity.has_component<LayerComponent>())
				entity.remove<LayerComponent>();
			entity.assign<LayerComponent>(layer);
		}
		else
			std::cout << "Warning: Attempt to add an entity failed. Reason: " << entity << " does not have a CellComponent." << std::endl;
		return;
	}

	std::cout << "Warning: Attempt to add an entity failed. Reason: Layer " << layer << " does not exist. " << std::endl;
}

void LayerManager::removeFromLayer(int layer, Entity entity)
{
	if (m_layers.count(layer))
	{
		auto cellPosition = entity.component<CellComponent>();
		if (cellPosition)
			m_layers[layer]->remove(entity, cellPosition->x, cellPosition->y);
		else
			std::cout << "Warning: Attempt to remove an entity failed. Reason: " << entity << " does not have a CellComponent." << std::endl;
		return;
	}

	std::cout << "Warning: Attempt to remove an entity failed. Reason: Layer " << layer << " does not exist. " << std::endl;
}

void LayerManager::remove(Entity entity)
{
	auto cellPosition = entity.component<CellComponent>();
	if (!cellPosition)
		std::cout << "Warning: Attempt to remove an entity failed. Reason: " << entity << " does not have a CellComponent." << std::endl;

	for (auto& l : m_layers)
		l.second->remove(entity, cellPosition->x, cellPosition->y);
}

/**
 * @brief	Refreshes cell positions of entities. (Transform - Cell synchronization)
 */
void LayerManager::update()
{
	for (auto& l : m_layers)
	{
		EntityLayerPtr layer = l.second;
		EntityGrid grid = layer->getEntityGrid();

		for (int x = 0; x < layer->getWidth(); x++)
		{
			for (int y = 0; y < layer->getHeight(); y++)
			{
				EntityCollection collection = grid[x][y];

				for (auto& entity : collection)
				{
					auto cell = entity.component<CellComponent>();
					auto transform = entity.component<TransformComponent>();

					if (cell && transform)
					{
						int cellX = static_cast<int>(transform->x / GameConstants::CELL_WIDTH);
						int cellY = static_cast<int>(transform->y / GameConstants::CELL_HEIGHT);

						// If the cell changed then refresh the cell and grid
						if (cellX != cell->x || cellY != cell->y)
						{
							layer->remove(entity, cell->x, cell->y);

							cell->x = cellX;
							cell->y = cellY;

							layer->add(entity, cell->x, cell->y);
						}
					}
				}
			}
		}
	}
}

EntityCollection& LayerManager::getEntities(int layer, int cellX, int cellY)
{
	assert(m_layers.count(layer));

	return m_layers[layer]->get(cellX, cellY);
}

bool LayerManager::hasSolidBlock(int layer, int cellX, int cellY)
{
	for (auto& e : getEntities(layer, cellX, cellY))
		if (e.has_component<SolidBlockComponent>())
			return true;

	return false;
}
