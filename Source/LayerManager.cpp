#include "LayerManager.h"
#include <iostream>
#include "Components/CellComponent.h"
#include "Components/TransformComponent.h"
#include "GameConstants.h"
#include "Components/SolidBlockComponent.h"
#include "Components/LayerComponent.h"
#include "GameGlobals.h"

EntityLayer* LayerManager::createLayer(int width, int height, int layer)
{
	m_layers[layer] = std::make_shared<EntityLayer>(width, height, layer);

	return m_layers[layer].get();
}

LayerManager::~LayerManager()
{
	GameGlobals::events->unsubscribe<entityx::EntityDestroyedEvent>(*this);
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

EntityCollection LayerManager::getEntities(int layer, int cellX, int cellY)
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

bool LayerManager::isFree(int layer, int cellX, int cellY)
{
	return getEntities(layer, cellX, cellY).size() == 0;
}