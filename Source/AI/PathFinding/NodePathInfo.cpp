#include "NodePathInfo.h"
#include <assert.h>

void NodePathInfo::insert(NodePathInfo* at)
{
	assert(at);
	auto cur = at;

	while (estimatedTotalCost > cur->estimatedTotalCost)
		if (cur->next) 
			cur = cur->next; 
		else
		{
			prev = cur;
			cur->next = this;
			next = nullptr;
			return;
		}

	while (estimatedTotalCost < cur->estimatedTotalCost)
		if (cur->prev) 
			cur = cur->prev; 
		else
		{
			next = cur;
			cur->prev = this;
			prev = nullptr;
			return;
		}

	auto tmp = cur->next;

	if (!tmp)
	{
		prev = cur;
		cur->next = this;
		next = nullptr;
	}
	else
	{
		cur->next = this;
		tmp->prev = this;
		next = tmp;
		prev = cur;
	}
}

NodePathInfo* NodePathInfo::remove()
{
	if (prev)
		prev->next = next;

	if (next)
		next->prev = prev;

	return prev ? prev : next;
}