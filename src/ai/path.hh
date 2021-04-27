/// \file
#ifndef PATH_HH
#define PATH_HH


#include "stdafx.h"
#include "command_func.h"
#include <queue>
#include <unordered_map>


namespace EmpireAI
{
	/**
	 * Pathfinder class that uses the A* algorithm to find the shortest path of a potential road
	 * between two map tiles.
	 */
	class Path
	{
	public:

		/**
		 * Enum representing the status of the pathfinder.
		 */
		enum Status
		{
			IN_PROGRESS, ///< Still searching for a path.
			FOUND,       ///< A path has been found.
			UNREACHABLE  ///< Pathfinder was unable to find a path.
		};

		Path(const TileIndex start, const TileIndex end);
		Status find(const uint16_t max_node_count = DEFAULT_NODE_COUNT_PER_FIND);

	private:

		/**
		 * Path node representing one tile on the map.
		 */
	    struct Node
	    {
	        Node(TileIndex in_tile_index, int32 in_h)
	        : tile_index(in_tile_index), h(in_h)
	        {
	        }

	        Node()
	        : tile_index(0), h(0)
	        {}

	        /**
	         * Compare the cost of this node with another node. If the f cost is the same,
	         * consider the node with the lower h cost to be cheaper.
	         * @param other
	         * @return
	         */
	        bool operator>(const Node& other) const
	        {
	        	if(f == other.f && h > other.h)
				{
					return true;
				}

				return true ? f > other.f : false;
	        }

	        bool update_costs(const Node& adjacent_node);

	        TileIndex tile_index; ///< The tile that this node represents.
	        TileIndex previous_tile_index = INVALID_TILE; ///< The tile that directly preceeds this node in the current path.
	        int32 g = 0; ///< Cost of the path from the start node to this node.
	        int32 h; ///< Cost of the path from this node to the end node.
	        int32 f = -1; ///< Cost of the total path from start to end via this node.
	    };

		void parse_adjacent_tile(const Node& current_node, const int8 x, const int8 y);
		Node get_node(const TileIndex tile_index);
		Node cheapest_open_node(bool& success);
		bool nodes_can_connect_road(const Node& node_from, const Node& node_to);

		/// Check up to this many nodes per call of find() by default
		static const uint16 DEFAULT_NODE_COUNT_PER_FIND = 20;

		void open_node(const Node& node);
		void close_node(const Node& node);

		Status m_status;

		const TileIndex m_start_tile_index;
		const TileIndex m_end_tile_index;

		std::unordered_map<TileIndex, Node> m_closed_nodes; ///< The list of closed nodes.
		std::priority_queue<Node, std::vector<Node>, std::greater<Node>> m_open_nodes; ///< The list of open nodes.

	public:


		/**
		 * Once a path has been found, the iterator can be used to traverse the path.
		 */
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
