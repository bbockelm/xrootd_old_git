/******************************************************************************/
/*                                                                            */
/*                      X r d C m s P r e f . c c                             */
/*                                                                            */
/* (c) 2012 University of Nebraska-Lincoln                                     */
/*                            All Rights Reserved                             */
/*   Produced by Brian Bockelman                                              */
/*                                                                            */
/******************************************************************************/

#include "XrdCms/XrdCmsPref.hh"

SMask_t XrdCmsPref::do_SelectNodes(SMask_t available_nodes)
{
   SMask_t filtered_nodes = available_nodes;
   SMask_t previous_nodes = available_nodes;
   unsigned int index=0;
   // Continue applying the increasingly-specific preference mask
   // until we've gone through all levels or we eliminated all the possible
   // sources.
   while (filtered_nodes)
   {
      previous_nodes = filtered_nodes;
      filtered_nodes &= m_prefs[index];
      index++;
      if (index == MAX_PREF_LEVELS)
      {
         if (filtered_nodes) previous_nodes = filtered_nodes;
         break;
      }
   }
   return previous_nodes;
}

SMask_t XrdCmsPref::do_AdditionalNodesToQuery(SMask_t queried_nodes)
{
   unsigned int index = MAX_PREF_LEVELS-1;
   SMask_t nodes_to_query;
   while (index != 0)
   {
      nodes_to_query = m_prefs[index] & ~queried_nodes;
      if (nodes_to_query)
         return nodes_to_query;
      index--;
   }
   // In the standard case where m_prefs[0] = 0xFFFFFFFF, this "does
   // the right thing" if all nodes have already been queried.
   return 0;
}

