// Date:    27th January 2019
// Purpose: Implements an asynchronous multi threaded basic network server.
//
// Version control
// 27 Jan 2019 Duncan Camilleri           Initial development
//

#ifndef __SERVERASYNC_H__
#define __SERVERASYNC_H__

// Check for missing includes.
#ifndef __SERVER_H__
#error "serverasync.h: missing include - server"
#elif not defined _GLIBCXX_MUTEX
#error "serverasync.h: missing include - mutex"
#elif not defined _GLIBCXX_THREAD
#error "serverasync.h: missing include - thread"
#endif

//
// Main asynchronous server class
// Implements a tcp based listener which waits for and accepts connections
// on a separate thread as requested.
// Base class server.h implements some of the more basic server functionality.
//
class serverasync : public server
{
public:
   // Constructor/destructor
   serverasync() = delete;
   serverasync(const char* address = nullptr, unsigned short port = 0);
   virtual ~serverasync();

   // Initializations.
   virtual bool init();
   virtual bool term();

   // Accept connections (threaded non blocking call).
   virtual bool waitForClients();

protected:
   int mMaxTimeoutSec = 30;            // max timeout - 30 sec (incl. term())

   // Accepting thread - a thread that accepts clients can only be run once.
   once_flag mAcceptOnce;              // make sure only one thread is accepting
   thread mAcceptThread;               // thread that is accepting connections
   void acceptLoop();                  // threaded by waitForClients()
};

#endif         // __SERVERASYNC_H__
