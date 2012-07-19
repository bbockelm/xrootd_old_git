/******************************************************************************/
/*                                                                            */
/*                      X r d C m s P r e f . h h                             */
/*                                                                            */
/* (c) 2012 University of Nebraska-Lincoln                                     */
/*                            All Rights Reserved                             */
/*   Produced by Brian Bockelman                                              */
/*                                                                            */
/******************************************************************************/
#ifndef __XRD_CMS_PREF_H_
#define __XRD_CMS_PREF_H_

#include "XrdCms/XrdCmsTypes.hh"

#define MAX_PREF_LEVELS 4

/*
 * This class specifies a preference, given by an XMI-plugin, to one element of
 * a node set over another.
 *
 * This allows multiple levels of preferences (currently fixed to 4) to be
 * expressed.  The higher the preference level, the more preferred and exclusive
 * it should be.
 */
class XrdCmsPref {

public:

inline XrdCmsPref()
   : m_is_initialized(false)
{
    // Defaults: every node has the same preference level, 0.
    m_prefs[0] = -1;
    for (unsigned int i=1; i<MAX_PREF_LEVELS; i++)
       m_prefs[i] = 0;
}

/*
 * A layer of indirection to make the prefs a no-op in the case of no plugins.
 */
inline SMask_t SelectNodes(SMask_t available_nodes)
{
   if (m_is_initialized)
   {
      return do_SelectNodes(available_nodes);
   }
   return available_nodes;
}

inline SMask_t AdditionalNodesToQuery(SMask_t queried_nodes)
{
   if (m_is_initialized)
   {
      return do_AdditionalNodesToQuery(queried_nodes);
   }
   return ~queried_nodes;
}

/*
 * Sets the given preference level.
 */
inline bool SetPreference(unsigned int level, SMask_t preference)
{
   m_is_initialized = true;
   if (level >= MAX_PREF_LEVELS)
       return false;
   m_prefs[level] = preference;
   return true;
}

private:

/*
 * Given a mask of the nodes which can be selected, filter according to the
 * current preferences.
 */
SMask_t do_SelectNodes(SMask_t available_nodes);

/*
 * Given the set of nodes which have already been queried, possibly
 * select some additional ones based on the preference mask.
 */
SMask_t do_AdditionalNodesToQuery(SMask_t queried_nodes);

bool    m_is_initialized;
SMask_t m_prefs[MAX_PREF_LEVELS];

};

#endif
