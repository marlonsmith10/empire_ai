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

		Path(TileIndex start, TileIndex end);
		~Path();

		// Find a partial path from start to end, returning true if the full path has been found
		Status find(uint16_t max_node_count = DEFAULT_NODE_COUNT_PER_FIND);

	private:

	    struct Node
	    {
	        Node(TileIndex in_tile_index, int32 in_h)
	        : tile_index(in_tile_index), h(in_h)
	        {
	        }

	        Node(){}

	        bool update_costs(Node& adjacent_node);

	        TileIndex tile_index;
	        TileIndex previous_tile_index = 0;
	        int32 g = 0;
	        int32 h;
	        int32 f = -1;

	        // Compare the f cost of two nodes
	        bool operator>(const Node& other) const
	        {
	        	if(f == other.f && h > other.h)
	        	{
	        		return true;
	        	}

	        	if(f > other.f)
	        	{
	        		return true;
	        	}

	        	return false;
	        }
	    };

		void parse_adjacent_tile(Node& current_node, int8 x, int8 y);

		// Return the corresponding node or create a new one if none is found
		Node get_node(TileIndex tile_index);

		// Check this many nodes per call of find()
		static const uint16 DEFAULT_NODE_COUNT_PER_FIND = 20;

		void open_node(Node& node);
		void close_node(Node& node);

		Status m_status;

		TileIndex m_start_tile_index;
		TileIndex m_end_tile_index;

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
            return Iterator(m_closed_nodes, m_end_tile_index);
        }

		Iterator end()
        {
            return Iterator(m_closed_nodes, m_start_tile_index);
        }
	};
}


#endif // PATH_HH
