#include "path.hh"

#include "script_map.hpp"
#include "script_road.hpp"
#include "script_tile.hpp"

#include <algorithm>
#include <iostream>

using namespace EmpireAI;


Path::Path(TileIndex start, TileIndex end)
: m_start_tile_index(start), m_end_tile_index(end)
{
	// Create an open node at the start
	Node current_node = get_node(start);
    current_node.f = current_node.h;
    current_node.previous_tile_index = current_node.tile_index;
	open_node(current_node);

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
		// Get the cheapest open node
		bool open_node_available = false;
		Node current_node = cheapest_open_node(open_node_available);

		// If there are no open nodes, the path is unreachable
		if(!open_node_available)
		{
			m_status = UNREACHABLE;
			break;
		}

	    //std::cout << "\nCurrent location: " << TileX(current_node.tile_index) << ", " << TileY(current_node.tile_index) << std::flush;
	    //std::cout << "  f: " << current_node.f << " g(from start): " << current_node.g << " h(from end) " << current_node.h << std::flush;
	    //std::cout << " from node: " << TileX(current_node.previous_tile_index) << ", " << TileY(current_node.previous_tile_index) << std::flush;

	    // Bulldoze to indicate where we're searching
	    //DoCommand(current_node.tile_index, 0, 0, DC_EXEC, CMD_LANDSCAPE_CLEAR);

        // Mark the current node as closed
	    close_node(current_node);

	    // If we've reached the destination, return true
	    if(current_node.tile_index == m_end_tile_index)
	    {
	    	m_status = FOUND;
	        break;
	    }

        // Calculate the f, h, g, values of the 4 surrounding nodes
	    parse_adjacent_tile(current_node, 1, 0);
	    parse_adjacent_tile(current_node, -1, 0);
	    parse_adjacent_tile(current_node, 0, 1);
	    parse_adjacent_tile(current_node, 0, -1);
	}

	return m_status;
}


void Path::parse_adjacent_tile(Node& current_node, int8 x, int8 y)
{
    TileIndex adjacent_tile_index = current_node.tile_index + ScriptMap::GetTileIndex(x, y);

    Node adjacent_node = get_node(adjacent_tile_index);

    // Create a node for this tile only if it is buildable
    if(ScriptTile::IsBuildable(adjacent_tile_index) || ScriptRoad::IsRoadTile(adjacent_tile_index))
    {
        if(adjacent_node.update_costs(current_node))
        {
            open_node(adjacent_node);
        }
    }
    else
    {
    	close_node(adjacent_node);
    }
}


Path::Node Path::cheapest_open_node(bool& out_success)
{
	// While there are open nodes available
	while(!m_open_nodes.empty())
	{
		// Remove the cheapest node from the open nodes list
		Node current_node = m_open_nodes.top();
		m_open_nodes.pop();

		// If this node has already been closed, skip to the next one
		if(m_closed_nodes.find(current_node.tile_index) != m_closed_nodes.end())
		{
			continue;
		}

		out_success = true;
		return current_node;
	}

	// There are no more open nodes
	out_success = false;
	return Node();
}


Path::Node Path::get_node(TileIndex tile_index)
{
    // If the node is not closed, create a new one.
	// Duplicate open nodes are considered an acceptable tradeoff since it's not easy to search std::priority_queue for
	// an already existing open node
    if(m_closed_nodes.find(tile_index) == m_closed_nodes.end())
    {
    	Node node(tile_index, ScriptMap::DistanceManhattan(tile_index, m_end_tile_index));
    	return node;
    }

    Node node = m_closed_nodes.at(tile_index);
    return node;
}


void Path::open_node(Node& node)
{
	// Push the node into the open node list. Does not check open nodes, instead allowing
	// duplicates to be created in the open node priority queue, since checking for already open nodes is slower
	// than just processing a node twice
	m_open_nodes.push(node);

	// Remove the node from the closed list
	m_closed_nodes.erase(node.tile_index);
}


void Path::close_node(Node& node)
{
    m_closed_nodes[node.tile_index] = node;
}


bool Path::Node::update_costs(Node& adjacent_node)
{
    int32 new_g = adjacent_node.g + 1;

    int32 new_f = new_g + h;

    // If this node is closed but cheaper than it was via previous path, or
    // if this is a new node (f == -1), return true to indicate the node should be opened again
    if(new_f < f || f == -1)
    {
        g = new_g;
        f = new_f;
        previous_tile_index = adjacent_node.tile_index;
        return true;
    }

    return false;
}
