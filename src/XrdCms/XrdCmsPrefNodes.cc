
/*
 * Due to the cost of holding the nodes lock while calling out to XrdCmsPref,
 * the relevant information is passed to this object instead.
 *
 * This data structure is meant to be highly optimized - eventually.  For
 * now, it is fairly straightforward.
 *
 */

#include "XrdCms/XrdCmsPrefNodes.hh"
#include "XrdCms/XrdCmsNode.hh"

XrdCmsPrefNodes::XrdCmsPrefNodes()
{
   for (unsigned int idx=0; idx<XRD_MAX_NODES; idx++)
      m_node_info[idx*XRD_MAX_NODE_SIZE] = '\0';
}

XrdCmsPrefNodes::~XrdCmsPrefNodes()
{
}

/*
* Copy information from the nodes data structure into this one.
*/
int XrdCmsPrefNodes::Acquire(unsigned int node_number, const XrdCmsNode &node)
{
   if (node_number >= XRD_MAX_NODES)
      return 1;
   if (node.getName(m_node_info + node_number*XRD_MAX_NODE_SIZE, XRD_MAX_NODE_SIZE-1) == 0)
      return 1;
   return 0;
}

