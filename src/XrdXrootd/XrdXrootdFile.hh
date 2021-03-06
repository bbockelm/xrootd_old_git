#ifndef _XROOTD_FILE_H_
#define _XROOTD_FILE_H_
/******************************************************************************/
/*                                                                            */
/*                      X r d X r o o t d F i l e . h h                       */
/*                                                                            */
/* (c) 2004 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC02-76-SFO0515 with the Department of Energy              */
/*                                                                            */
/* This file is part of the XRootD software suite.                            */
/*                                                                            */
/* XRootD is free software: you can redistribute it and/or modify it under    */
/* the terms of the GNU Lesser General Public License as published by the     */
/* Free Software Foundation, either version 3 of the License, or (at your     */
/* option) any later version.                                                 */
/*                                                                            */
/* XRootD is distributed in the hope that it will be useful, but WITHOUT      */
/* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or      */
/* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public       */
/* License for more details.                                                  */
/*                                                                            */
/* You should have received a copy of the GNU Lesser General Public License   */
/* along with XRootD in a file called COPYING.LESSER (LGPL license) and file  */
/* COPYING (GPL license).  If not, see <http://www.gnu.org/licenses/>.        */
/*                                                                            */
/* The copyright holder's institutional names and contributor's names may not */
/* be used to endorse or promote products derived from this software without  */
/* specific prior written permission of the institution or contributor.       */
/******************************************************************************/

#include <string.h>

#include "XProtocol/XPtypes.hh"

/******************************************************************************/
/*                              x r d _ F i l e                               */
/******************************************************************************/

class XrdSfsFile;
class XrdXrootdFileLock;
class XrdXrootdMonitor;

class XrdXrootdFile
{
public:

XrdSfsFile  *XrdSfsp;           // -> Actual file object
char        *mmAddr;            // Memory mapped location, if any
long long    fSize;             // File size at time of object creation
int          fdNum;             // File descriptor number if regular file
kXR_unt32    FileID;            // Unique file id used for monitoring
char         FileKey[34];       // -> Unique hash name for the file
char         Reserved[2];
char         FileMode;          // 'r' or 'w'
char         AsyncMode;         // 1 -> if file in async r/w mode
char         isMMapped;         // 1 -> file is memory mapped
char         sfEnabled;         // 1 -> file is sendfile enabled
char        *ID;                // File user
long long    readCnt;
long long    writeCnt;

static void Init(XrdXrootdFileLock *lp, int sfok) {Locker = lp; sfOK = sfok;}

           XrdXrootdFile(char *id, XrdSfsFile *fp, char mode='r', 
                         char async='\0', int sfOK=0, struct stat *sP=0);
          ~XrdXrootdFile();

private:
int bin2hex(char *outbuff, char *inbuff, int inlen);
static XrdXrootdFileLock *Locker;
static int                sfOK;
static const char        *TraceID;
};
 
/******************************************************************************/
/*                      x r o o t d _ F i l e T a b l e                       */
/******************************************************************************/

// The before define the structure of the file table. We will have FTABSIZE
// internal table entries. We will then provide an external linear table
// that increases by FTABSIZE entries. There is one file table per link and
// it is owned by the base protocol object.
//
#define XRD_FTABSIZE   16
  
// WARNING! Manipulation (i.e., Add/Del/delete) of this object must be
//          externally serialized at the link level. Only one thread
//          may be active w.r.t this object during manipulation!
//
class XrdXrootdFileTable
{
public:

       int            Add(XrdXrootdFile *fp);

       void           Del(int fnum);

inline XrdXrootdFile *Get(int fnum)
                         {if (fnum >= 0)
                             {if (fnum < XRD_FTABSIZE) return FTab[fnum];
                              if (XTab && (fnum-XRD_FTABSIZE)<XTnum)
                                 return XTab[fnum-XRD_FTABSIZE];
                             }
                          return (XrdXrootdFile *)0;
                         }

       void            Recycle(XrdXrootdMonitor *monP=0, int doDel=1);

       XrdXrootdFileTable() {memset((void *)FTab, 0, sizeof(FTab));
                             FTfree = 0; XTab = 0; XTnum = XTfree = 0;
                            }
      ~XrdXrootdFileTable() {Recycle(0, 0);}

private:

static const char *TraceID;

XrdXrootdFile *FTab[XRD_FTABSIZE];
int            FTfree;

XrdXrootdFile **XTab;
int             XTnum;
int             XTfree;
};
#endif
