#ifndef PATH_HH
#define PATH_HH


#include "stdafx.h"
#include "command_func.h"
#include <queue>
#include <unordered_map>


namespace EmpireAI
{
	class Path
	{
	public:

		enum Status
		{
			IN_PROGRESS,
			FOUND,
			UNREACHABLE
		};

		Path(const TileIndex start, const TileIndex end);

		// Find a partial path from start to end, returning true if the full path has been found
		Status find(const uint16_t max_node_count = DEFAULT_NODE_COUNT_PER_FIND);

	private:

	    struct Node
	    {
	        Node(TileIndex in_tile_index, int32 in_h)
	        : tile_index(in_tile_index), h(in_h)
	        {
	        }

	        Node()
	        : tile_index(0), h(0)
	        {}

	        bool operator>(const Node& other) const
	        {
	        	if(f == other.f && h > other.h)
				{
					return true;
				}

				return true ? f > other.f : false;
	        }

	        // Update the Node's g and h values, as well as its previous node. Returns true if the
	        // new values are lower than the previous ones.
	        bool update_costs(const Node& adjacent_node);

	        TileIndex tile_index;
	        TileIndex previous_tile_index = INVALID_TILE;
	        int32 g = 0;
	        int32 h;
	        int32 f = -1;
	    };

		void parse_adjacent_tile(const Node& current_node, const int8 x, const int8 y);

		// Return the corresponding node or create a new one if none is found
		Node get_node(const TileIndex tile_index);

		// Get the cheapest open node, returns nullptr if there are no open nodes
		Node cheapest_open_node(bool& success);

		// Returns true if a road can be built from one node to the next
		bool nodes_can_connect_road(const Node& node_from, const Node& node_to);

		// Check this many nodes per call of find()
		static const uint16 DEFAULT_NODE_COUNT_PER_FIND = 20;

		void open_node(const Node& node);
		void close_node(const Node& node);

		Status m_status;

		const TileIndex m_start_tile_index;
		const TileIndex m_end_tile_index;

		// Containers for open and closed nodes
		std::unordered_map<TileIndex, Node> m_closed_nodes;
		std::priority_queue<Node, std::vector<Node>, std::greater<Node>> m_open_nodes;

	public:


        class Iterator
        {
        public:

            Iterator(std::unordered_map<TileIndex, Node>& node_map, TileIndex tile_index)
            : m_node_map(node_map), m_tile_index(tile_index)
            {}

            bool operator==(const Iterator& iterator) const
            {
                return m_tile_index == iterator.m_tile_index;
            }

            bool operator!=(const Iterator& iterator) const
            {
                return m_tile_index != iterator.m_tile_index;
            }

            const Iterator& operator=(const TileIndex tile_index)
            {
            	m_tile_index = tile_index;
                return *this;
            }

            const Iterator& operator++()
            {
            	m_tile_index = m_node_map[m_tile_index].previous_tile_index;
                return *this;
            }

            Iterator operator++(int)
            {
                Iterator iterator = *this;
                m_tile_index = m_node_map[m_tile_index].previous_tile_index;
                return iterator;
            }

            TileIndex operator*() const
            {
                return m_tile_index;
            }

        private:
            std::unordered_map<TileIndex, Node>& m_node_map;
            TileIndex m_tile_index;
        };

		Iterator begin()
        {
			// Path is traversed in reverse order of discovery, thus begin returns the end tile
            return Iterator(m_closed_nodes, m_end_tile_index);
        }

		Iterator end()
        {
			// Path is traversed in reverse order of discovery, thus end returns one past the start tile
            return Iterator(m_closed_nodes, INVALID_TILE);
        }
	};
}


#endif // PATH_HH
