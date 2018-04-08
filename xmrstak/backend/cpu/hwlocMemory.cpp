#include "xmrstak/backend/cpu/hwlocMemory.hpp"

#ifndef CONF_NO_HWLOC

#include "xmrstak/misc/console.hpp"
#include "xmrstak/cli/colors.hpp"

#include <hwloc.h>

/** pin memory to NUMA node
 *
 * Set the default memory policy for the current thread to bind memory to the
 * NUMA node.
 *
 * @param puId core id
 */
void bindMemoryToNUMANode( size_t puId )
{
    int depth;
    hwloc_topology_t topology;

    hwloc_topology_init(&topology);
    hwloc_topology_load(topology);

    if(!hwloc_topology_get_support(topology)->membind->set_thisthread_membind)
    {
        #ifdef EXTRAWARNINGS
        printer::inst()->print_msg(L0, YELLOW("hwloc: set_thisthread_membind not supported"));
        #endif
        hwloc_topology_destroy(topology);
        return;
    }

    depth = hwloc_get_type_depth(topology, HWLOC_OBJ_PU);

    for( size_t i = 0;
        i < hwloc_get_nbobjs_by_depth(topology, depth);
        i++ )
    {
        hwloc_obj_t pu = hwloc_get_obj_by_depth(topology, depth, i);
        if(  pu->os_index == puId )
        {
            if( 0 > hwloc_set_membind_nodeset(
                topology,
                pu->nodeset,
                HWLOC_MEMBIND_BIND,
                HWLOC_MEMBIND_THREAD))
            {
                #ifdef EXTRAWARNINGS
                printer::inst()->print_msg(L0, YELLOW("hwloc: can't bind memory, continuing"));
                #endif
            }
            else
            {
                printer::inst()->print_msg(L0, GREEN("hwloc: memory pinned to NUMA mode"));
                break;
            }
        }
    }

    hwloc_topology_destroy(topology);
}
#else

void bindMemoryToNUMANode( size_t )
{
}

#endif
