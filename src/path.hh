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

	        Node()
	        : tile_index(0), h(0)
	        {}

	        // Update the Node's g and h values, as well as its previous node. Returns true if the
	        // new values are lower than the previous ones.
	        bool update_costs(Node* adjacent_node);

	        const TileIndex tile_index;
	        Node* previous_node = nullptr;
	        int32 g = 0;
	        const int32 h;
	        int32 f = -1;
	    };

	    struct NodeCostCompare
	    {
	    	bool operator()(const Node* node1, const Node* node2)
			{
				return node1->f > node2->f;
			}
	    };


		void parse_adjacent_tile(Node* current_node, int8 x, int8 y);

		// Return the corresponding node or create a new one if none is found
		Node* get_node(TileIndex tile_index);

		// Get the cheapest open node, returns nullptr if there are no open nodes
		Node* cheapest_open_node();

		// Check this many nodes per call of find()
		static const uint16 DEFAULT_NODE_COUNT_PER_FIND = 20;

		void open_node(Node* node);
		void close_node(Node* node);

		Status m_status;

		Node* m_start_node;
		Node* m_current_node;
		TileIndex m_end_tile_index;

		// Containers for open and closed nodes. Node pointers are stored here instead of Node objects
		// for several reasons:
		// 1. Iterator can use nullptr to determine the Node before the start Node, which can't be
		// done using TileIndex since all values of TileIndex are valid.
		// 2. Node member variables can be marked as const, which can't be done if they are stored
		// directly in the containers.
		// 3. cheapest_open_node can return nullptr instead of having to take in and modify a status variable.
		std::unordered_map<TileIndex, Node*> m_closed_nodes;
		std::priority_queue<Node*, std::vector<Node*>, NodeCostCompare> m_open_nodes;

	public:

		class Iterator
        {
        public:

            Iterator(const Path::Node* node)
            : m_iterator_node(node)
            {}

            bool operator==(const Iterator& iterator) const
            {
                return m_iterator_node == iterator.m_iterator_node;
            }

            const Iterator& operator=(const Path::Node* node)
            {
                m_iterator_node = node;
                return *this;
            }

            bool operator!=(const Iterator& iterator) const
            {
                return m_iterator_node != iterator.m_iterator_node;
            }

            const Iterator& operator++()
            {
                m_iterator_node = m_iterator_node->previous_node;
                return *this;
            }

            Iterator operator++(int)
            {
                Iterator iterator = *this;
                m_iterator_node = m_iterator_node->previous_node;
                return iterator;
            }

            TileIndex operator*() const
            {
                if(m_iterator_node == nullptr)
                {
                    return 0;
                }

                return m_iterator_node->tile_index;
            }

        private:
            const Path::Node* m_iterator_node;
        };

		Iterator begin()
        {
            return Iterator(m_current_node);
        }

		Iterator end()
        {
            return Iterator(m_start_node);
        }
	};
}


#endif // PATH_HH
