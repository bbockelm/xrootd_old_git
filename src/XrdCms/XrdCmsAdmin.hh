#ifndef __CMSADMIN__
#define __CMSADMIN__
/******************************************************************************/
/*                                                                            */
/*                        X r d C m s A d m i n . h h                         */
/*                                                                            */
/* (c) 2007 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*                            All Rights Reserved                             */
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

#include <stdlib.h>
#include <sys/uio.h>

#include "XrdCms/XrdCmsProtocol.hh"
#include "XrdCms/XrdCmsRRData.hh"
#include "XrdOuc/XrdOucStream.hh"
#include "XrdSys/XrdSysPthread.hh"

class XrdNetSocket;

class XrdCmsAdmin
{
public:

       void  Login(int socknum);

static void  setSync(XrdSysSemaphore  *sync)  {SyncUp = sync;}

       void *Notes(XrdNetSocket *AdminSock);

static void  Relay(int setSock, int newSock);

       void  Send(const char *Req, XrdCmsRRData &Data);

       void *Start(XrdNetSocket *AdminSock);

      XrdCmsAdmin() {Sname = 0; Stype = "Server"; Primary = 0;}
     ~XrdCmsAdmin() {if (Sname) free(Sname);}

private:

int   do_Login();
void  do_RmDid(int dotrim=0);
void  do_RmDud(int dotrim=0);

static XrdSysMutex      myMutex;
static XrdSysSemaphore *SyncUp;
static int              POnline;
       XrdOucStream     Stream;
       const char      *Stype;
       char            *Sname;
       int              Primary;
};
#endif
