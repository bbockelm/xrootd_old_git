
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
	: m_node_info(NULL) // Set to null in case "new" throws an exception.
{
   m_node_info = new char[XRD_MAX_NODES*XRD_MAX_NODE_SIZE];
   memset(m_node_info, 0, XRD_MAX_NODES*XRD_MAX_NODE_SIZE);
}

XrdCmsPrefNodes::~XrdCmsPrefNodes()
{
   if (m_node_info)
      delete [] m_node_info;
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

/*
* Get information about a given node.
*/
const char * XrdCmsPrefNodes::GetNodeName(unsigned int node_number)
{
   if (node_number >= XRD_MAX_NODES)
      return NULL;
   return m_node_info + node_number*XRD_MAX_NODE_SIZE;
}

