// Date:    27th January 2019
// Purpose: Implements a synchronous single threaded basic network server.
//
// Version control
// 27 Jan 2019 Duncan Camilleri           Initial development
// 12 Mar 2019 Duncan Camilleri           Introduced user read fd's processing
//

#ifndef __SERVERSYNC_H__
#define __SERVERSYNC_H__

// Check for missing includes.
#ifndef __SERVER_H__
#error "serversync.h: missing include - server"
#elif not defined _GLIBCXX_MUTEX
#error "serversync.h: missing include - mutex"
#endif

//
// Main asynchronous server class
// Implements a tcp based listener which waits for and accepts connections.
// Base class server.h implements some of the more basic server functionality.
//
class serversync : public server
{
public:
   // Constructor/destructor
   serversync() = delete;
   serversync(const char* address = nullptr, unsigned short port = 0);
   virtual ~serversync();

   // Initializations.
   virtual bool init();
   virtual bool term();

   // Accept connections (blocking call).
   virtual bool waitForClients();

protected:
   int mMaxTimeoutSec = 30;            // max timeout - 30 sec (incl. term())

   // Accepting loop.
   bool sessionStop = false;           // indicates the accept loop terminated
   once_flag mAcceptOnce;              // make sure waiting happens only once!
   void selectLoop();                  // called once by waitForClients()
   bool selectProcess(fd_set* pfd);
};

#endif         // __SERVERSYNC_H__
