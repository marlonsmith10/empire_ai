#include "path.hh"

#include "script_map.hpp"
#include "script_road.hpp"
#include "script_tile.hpp"
#include "map_func.h"

#include <algorithm>

using namespace EmpireAI;


Path::Path(const TileIndex start, const TileIndex end)
: m_end_tile_index(end)
{
	// Create an open node at the start
	std::unique_ptr<Node> start_node = get_node(start);
	start_node->f = start_node->h;

	// Keep a pointer to the start node, for use by the iterator once a path has been found
	m_start_node = start_node.get();

	open_node(std::move(start_node));

	m_status = IN_PROGRESS;
}


Path::Status Path::find(const uint16_t max_node_count)
{
    if(m_status != IN_PROGRESS)
    {
        return m_status;
    }

	// While not at end of path
	for(uint16 node_count = 0; node_count < max_node_count; node_count++)
	{
		// Get the cheapest open node
		std::unique_ptr<Node> current_node = cheapest_open_node();

		// If there are no open nodes, the path is unreachable
		if(current_node == nullptr)
		{
			m_status = UNREACHABLE;
			break;
		}

	    // If we've reached the destination, return true
	    if(current_node->tile_index == m_end_tile_index)
	    {
		    // Keep a pointer to the end node, for use by the iterator
		    m_end_node = current_node.get();
	    	close_node(std::move(current_node));
	    	m_status = FOUND;
	        break;
	    }

        // Calculate the f, h, g, values of the 4 surrounding nodes
	    parse_adjacent_tile(current_node.get(), 1, 0);
	    parse_adjacent_tile(current_node.get(), -1, 0);
	    parse_adjacent_tile(current_node.get(), 0, 1);
	    parse_adjacent_tile(current_node.get(), 0, -1);

        // Mark the current node as closed
	    close_node(std::move(current_node));
	}

	return m_status;
}


void Path::parse_adjacent_tile(Node* const current_node, const int8 x, const int8 y)
{
    TileIndex adjacent_tile_index = current_node->tile_index + ScriptMap::GetTileIndex(x, y);

    std::unique_ptr<Node> adjacent_node = get_node(adjacent_tile_index);

    // Check to see if this tile can be used as part of the path
    if(nodes_can_connect_road(current_node, adjacent_node.get()))
    {
        if(adjacent_node->update_costs(current_node))
        {
            open_node(std::move(adjacent_node));
        }
        else
		{
			close_node(std::move(adjacent_node));
		}
    }
    else
    {
    	close_node(std::move(adjacent_node));
    }
}


bool Path::nodes_can_connect_road(const Node* const node_from, const Node* const node_to) const
{
	// The start node doesn't connect to a previous node, so we can't check it for the correct slope.
	// The pathfinder can only ensure that the next node in the path can connect to the start node.
	if(node_from->previous_node == nullptr)
	{
		return true;
	}

	int32 supports_road = ScriptRoad::CanBuildConnectedRoadPartsHere(node_from->tile_index, node_from->previous_node->tile_index, node_to->tile_index);

	if(supports_road <= 0)
	{
		return false;
	}

	if(!ScriptTile::IsBuildable(node_to->tile_index) && !ScriptRoad::IsRoadTile(node_to->tile_index))
	{
		return false;
	}

	return true;
}


std::unique_ptr<Path::Node> Path::cheapest_open_node()
{
	// While there are open nodes available
	while(!m_open_nodes.empty())
	{
		// Remove the cheapest node from the open nodes list
		std::unique_ptr<Node> current_node = std::move(const_cast<std::unique_ptr<Node>&>(m_open_nodes.top()));
		m_open_nodes.pop();

		// If this node has already been closed, discard it and skip to the next one. Duplicates are expected
		// here because get_node() doesn't check for duplicates for performance reasons.
		if(m_closed_nodes.find(current_node->tile_index) != m_closed_nodes.end())
		{
			continue;
		}

		return current_node;
	}

	// There are no more open nodes
	return nullptr;
}


std::unique_ptr<Path::Node> Path::get_node(const TileIndex tile_index)
{
    // If the node is not closed, create a new one.
	// Duplicate open nodes are considered an acceptable tradeoff since it's not easy to search std::priority_queue for
	// an already existing open node
    if(m_closed_nodes.find(tile_index) == m_closed_nodes.end())
    {
    	return std::unique_ptr<Node>(new Node(tile_index, ScriptMap::DistanceManhattan(tile_index, m_end_tile_index)));
    }

    std::unique_ptr<Node> node = std::move(m_closed_nodes.at(tile_index));

	// Remove the (now null) node from the closed list
	m_closed_nodes.erase(tile_index);

    return node;
}


void Path::open_node(std::unique_ptr<Node> node)
{
	// Push the node into the open node list. Does not check open nodes, instead allowing
	// duplicates to be created in the open node priority queue, since checking for already open nodes is slower
	// than just processing a node twice.
	m_open_nodes.push(std::move(node));
}


void Path::close_node(std::unique_ptr<Node> node)
{
    m_closed_nodes[node->tile_index] = std::move(node);
}


bool Path::Node::update_costs(Node* const adjacent_node)
{
    int32 new_g = adjacent_node->g + 1;

    int32 new_f = new_g + h;

    // If this node is closed but cheaper than it was via previous path, or
    // if this is a new node (f == -1), return true to indicate the node should be opened again
    if(new_f < f || f == -1)
    {
        g = new_g;
        f = new_f;
        previous_node = adjacent_node;

        return true;
    }

    return false;
}
