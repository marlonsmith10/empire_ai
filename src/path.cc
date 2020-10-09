#include "path.hh"

#include "script_map.hpp"
#include "script_road.hpp"
#include "script_tile.hpp"

#include <algorithm>
#include <iostream>

using namespace EmpireAI;


Path::Path(TileIndex start, TileIndex end)
{
	// Save start node
    m_current_node = m_start_node = get_node(start);
	m_current_node->update_costs(m_current_node);
	open_node(m_current_node);

	m_end_tile_index = end;

	m_status = IN_PROGRESS;
}


Path::Status Path::find(uint16_t max_node_count)
{
    if(m_status != IN_PROGRESS)
    {
        return m_status;
    }

	// While not at end of path
	for(uint16 node_count = 0; node_count < max_node_count; node_count++)
	{
	    // Find the cheapest open node
	    m_current_node = cheapest_open_node();

	    // If there are no more open nodes, the path can't be found
	    if(m_current_node == nullptr)
	    {
	    	m_status = UNREACHABLE;
			break;
	    }

        // Mark the current node as closed
	    close_node(m_current_node);

	    // If we've reached the destination, return true
	    if(m_current_node->tile_index == m_end_tile_index)
	    {
	    	m_status = FOUND;
	        break;
	    }

        // Calculate the f, h, g, values of the 4 surrounding nodes
	    parse_adjacent_tile(m_current_node, 1, 0);
	    parse_adjacent_tile(m_current_node, -1, 0);
	    parse_adjacent_tile(m_current_node, 0, 1);
	    parse_adjacent_tile(m_current_node, 0, -1);
	}

	return m_status;
}


void Path::parse_adjacent_tile(Node* current_node, int8 x, int8 y)
{
    TileIndex adjacent_tile_index = current_node->tile_index + ScriptMap::GetTileIndex(x, y);

    // Create a node for this tile only if it is buildable
    if(ScriptTile::IsBuildable(adjacent_tile_index) || ScriptRoad::IsRoadTile(adjacent_tile_index))
    {
        Node* adjacent_node = get_node(adjacent_tile_index);
        if(adjacent_node->update_costs(current_node))
        {
            open_node(adjacent_node);
        }
    }
}


Path::Node* Path::get_node(TileIndex tile_index)
{
	// Return the node if it already exists
	for(Node* node : m_open_nodes)
	{
		if(node->tile_index == tile_index)
		{
			return node;
		}
	}

    // Return the node if it already exists
    for(Node* node : m_closed_nodes)
    {
        if(node->tile_index == tile_index)
        {
            return node;
        }
    }

	Node* node = new Node(tile_index, ScriptMap::DistanceManhattan(tile_index, m_end_tile_index));
    m_open_nodes.push_back(node);

    return node;
}


Path::Node* Path::cheapest_open_node()
{
	if(m_open_nodes.empty())
	{
		return nullptr;
	}

    // Get the first open node to start
    Node* cheapest_open_node = m_open_nodes.front();

    // Compare the first open node to all other open nodes to find the cheapest
    for(Node* node : m_open_nodes)
    {
        if(node->f < cheapest_open_node->f)
        {
            cheapest_open_node = node;
        }

        // Break ties by choosing closest to destination
        if(node->f == cheapest_open_node->f && node->h < cheapest_open_node->h)
        {
            cheapest_open_node = node;
        }
    }

    return cheapest_open_node;
}


void Path::open_node(Node* node)
{
    // Find the node in the list of closed nodes
    auto it = std::find(m_closed_nodes.begin(), m_closed_nodes.end(), node);

    if(it != m_closed_nodes.end())
    {
        node->open = true;
        m_open_nodes.push_back(node);
        m_closed_nodes.erase(it);
    }
}


void Path::close_node(Node* node)
{
    // Find the node in the list of open nodes
    auto it = std::find(m_open_nodes.begin(), m_open_nodes.end(), node);

    if(it != m_open_nodes.end())
    {
        node->open = false;
        m_closed_nodes.push_back(node);
        m_open_nodes.erase(it);
    }
}


bool Path::Node::update_costs(Node* adjacent_node)
{
    int32 new_g = adjacent_node->g + 1;

    int32 new_f = new_g + h;

    if(new_f < f || f == -1)
    {
        g = new_g;
        f = new_f;
        previous_node = adjacent_node;
        return true;
    }

    return false;
}


Path::~Path()
{
    for(Node* node : m_open_nodes)
    {
        delete node;
    }

    for(Node* node : m_closed_nodes)
    {
        delete node;
    }
}
