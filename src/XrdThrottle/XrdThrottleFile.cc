
#include "XrdSfs/XrdSfsAio.hh"
#include "XrdSec/XrdSecEntity.hh"

#include "XrdThrottle.hh"

using namespace XrdThrottle;

#define DO_LOADSHED(amount) if (!m_redir && m_throttle.CheckLoadShed(amount, m_loadshed)) \
{ \
   unsigned port; \
   std::string host; \
   m_throttle.PerformLoadShed(m_loadshed, host, port); \
   TRACE(LOADSHED, "Performing load-shed for client " << m_user); \
   error.setErrInfo(port, host.c_str()); \
   m_redir = true; \
   return SFS_REDIRECT; \
}

#define DO_THROTTLE(amount) \
DO_LOADSHED(amount) \
m_throttle.Apply(amount, 1, m_uid); \
XrdThrottleTimer xtimer = m_throttle.StartIOTimer();

const char * File::TraceID = "XrdThrottleFile";

File::File(const char                     *user,
                 int                       monid,
                 std::auto_ptr<XrdSfsFile> sfs,
                 XrdThrottleManager       &throttle,
                 XrdOucTrace              &trace)
   : m_sfs(sfs), // Guaranteed to be non-null by FileSystem::newFile
     m_uid(0),
     m_redir(false),
     m_passAlongPreread(false),
     m_user(user),
     m_throttle(throttle),
     m_trace(trace)
{}

File::~File()
{}

int
File::open(const char                *fileName,
                 XrdSfsFileOpenMode   openMode,
                 mode_t               createMode,
           const XrdSecEntity        *client,
           const char                *opaque)
{
   m_uid = XrdThrottleManager::GetUid(client->name);
   m_throttle.PrepLoadShed(opaque, m_loadshed);
   DO_LOADSHED(1024*1024);
   return m_sfs->open(fileName, openMode, createMode, client, opaque);
}

int
File::close()
{
   return m_sfs->close();
}

int
File::fctl(const int               cmd,
           const char             *args,
                 XrdOucErrInfo    &out_error)
{
   // Disable sendfile
   if (cmd == SFS_FCTL_PREAD)
   {
      m_passAlongPreread = m_sfs->fctl(cmd, args, out_error) == SFS_OK;
      return SFS_OK;
   }
   else return m_sfs->fctl(cmd, args, out_error);
}

const char *
File::FName()
{
   return m_sfs->FName();
}

int
File::getMmap(void **Addr, off_t &Size)
{  // We cannot monitor mmap-based reads, so we disable them.
   return SFS_ERROR;
}

int
File::read(XrdSfsFileOffset   fileOffset,
           XrdSfsXferSize     amount)
{
   DO_THROTTLE(amount)
   if (m_passAlongPreread)
      return m_sfs->read(fileOffset, amount);
   else
      return SFS_OK;
}

XrdSfsXferSize
File::read(XrdSfsFileOffset   fileOffset,
           char              *buffer,
           XrdSfsXferSize     buffer_size)
{
   DO_THROTTLE(buffer_size);
   return m_sfs->read(fileOffset, buffer, buffer_size);
}

int
File::read(XrdSfsAio *aioparm)
{
   // Note that loadshed does not work for async requests due to 
   // state issues in the AIO objects.  Basically, the AIO sendError
   // method cannot get enough information to know how to respond with
   // the correct redirect information.
   m_throttle.Apply(aioparm->sfsAio.aio_nbytes, 1, m_uid);
   XrdThrottleTimer xtimer = m_throttle.StartIOTimer();
   aioparm->Result = m_sfs->read((XrdSfsFileOffset)aioparm->sfsAio.aio_offset,
                                           (char *)aioparm->sfsAio.aio_buf,
                                   (XrdSfsXferSize)aioparm->sfsAio.aio_nbytes);

   aioparm->doneRead();
   return SFS_OK;
}

XrdSfsXferSize
File::write(      XrdSfsFileOffset   fileOffset,
            const char              *buffer,
                  XrdSfsXferSize     buffer_size)
{
   // Cannot use DO_THROTTLE as upper layers cannot handle loadshed.
   //DO_THROTTLE(buffer_size);
   m_throttle.Apply(buffer_size, 1, m_uid);
   XrdThrottleTimer xtimer = m_throttle.StartIOTimer();

   return m_sfs->write(fileOffset, buffer, buffer_size);
}

int
File::write(XrdSfsAio *aioparm)
{
   m_throttle.Apply(aioparm->sfsAio.aio_nbytes, 1, m_uid);
   XrdThrottleTimer xtimer = m_throttle.StartIOTimer();

   aioparm->Result = this->write((XrdSfsFileOffset)aioparm->sfsAio.aio_offset,
                                           (char *)aioparm->sfsAio.aio_buf,
                                   (XrdSfsXferSize)aioparm->sfsAio.aio_nbytes);
   aioparm->doneRead();
   return SFS_OK;
}

int
File::sync()
{
   return m_sfs->sync();
}

int
File::sync(XrdSfsAio *aiop)
{
   aiop->Result = this->sync();
   aiop->doneWrite();
   return m_sfs->sync(aiop);
}

int
File::stat(struct stat *buf)
{
   return m_sfs->stat(buf);
}

int
File::truncate(XrdSfsFileOffset   fileOffset)
{
   return m_sfs->truncate(fileOffset);
}

int
File::getCXinfo(char cxtype[4], int &cxrsz)
{
   return m_sfs->getCXinfo(cxtype, cxrsz);
}

