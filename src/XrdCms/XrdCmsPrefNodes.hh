#ifndef __XRD_CMS_PREF_NODES_H_
#define __XRD_CMS_PREF_NODES_H_
/*
 * Due to the cost of holding the nodes lock while calling out to XrdCmsPref,
 * the relevant information is passed to this object instead.
 *
 * This data structure is meant to be highly optimized - eventually.  For
 * now, it is fairly straightforward, and probably somewhat slow.
 *
 */

#define XRD_MAX_NODES 64
#define XRD_MAX_NODE_SIZE 64

class XrdCmsCluster;
class XrdCmsNode;

class XrdCmsPrefNodes {

friend class XrdCmsCluster;

public:

   XrdCmsPrefNodes();

   ~XrdCmsPrefNodes();

   /*
    * Get information about a given node.
    */
   const char * GetNodeName(unsigned int node_number);

protected:
   /*
    * Copy information from the nodes data structure into this one.
    */
   int Acquire(unsigned int node_number, const XrdCmsNode &node);

private:

   char * m_node_info;

};
#endif
