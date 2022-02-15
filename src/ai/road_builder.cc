#include "road_builder.hh"
#include "openttd_functions.hh"

using namespace EmpireAI;


RoadBuilder::RoadBuilder(Path& path)
: m_path(path),
  m_previous_path_iterator(path.begin()),
  m_current_path_iterator(path.begin())
{

}


bool RoadBuilder::build_road_segment()
{
    // If the current and previous iterators are the same, we're at the beginning of the path
    if(m_current_path_iterator == m_previous_path_iterator)
    {
        m_current_path_iterator++;
    }

    // Build one segment of road
    if(m_current_path_iterator != m_path.end())
    {
        EmpireAI::build_road(*m_previous_path_iterator, *m_current_path_iterator);

        m_previous_path_iterator++;
        m_current_path_iterator++;
        return false;
    }

    return true;
}
