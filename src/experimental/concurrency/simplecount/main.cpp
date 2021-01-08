// Date:    12th April 2019
// Purpose: Concurrency tests - This is just a count and print test.
//          The counting thread increments the count by one only when printing
//          has been done. The printing happens only when count has incremented
//          and needs to be printing. Printing waits for the next count to occur
//          and counting waits for the next print to occur. 
//
// Version control
// 12 Apr 2019 Duncan Camilleri           Initial development
//

#include <assert.h>           // assert
#include <stdio.h>            // printf
#include <unistd.h>           // close
#include <string.h>           // strerror
#include <mutex>              // locking for queue
#include <condition_variable>
#include <vector>
#include <thread>             // logging thread
#include <errno.h>            // errno

using namespace std;

int gCount = 0;
mutex gCountLock;
condition_variable gOnCount;
bool gIncrement = false;
mutex gPrintLock;
condition_variable gOnPrint;
bool gPrint = false;

//
// GENERIC LOGGING
//

// Standard error log.
auto gErr = [](const char* const label, const char* const err) {
   printf("[error %s] %s (%d:%s)\n", label, err, errno, strerror(errno));
};

void lockstep()
{
   unique_lock<mutex> countlock(gCountLock, defer_lock);
   unique_lock<mutex> printlock(gPrintLock, defer_lock);
   while (true) {
      // Increment counter.
      printlock.lock();
      printf("%d\n", gCount);
      printlock.unlock();

      // Update state variables.
      gIncrement = false;
      gPrint = true;

      // Notify print complete.
      gOnPrint.notify_all();

      // Wait for counting before printing again.
      countlock.lock();
      while (!gIncrement)
         gOnCount.wait(countlock);
      countlock.unlock();
   };
}

//
// MAIN APP
//

int main(int argc, char** argv)
{
   thread t(lockstep);

   unique_lock<mutex> countlock(gCountLock, defer_lock);
   unique_lock<mutex> printlock(gPrintLock, defer_lock);
   while (true) {
      // Increment counter.
      countlock.lock();
      gCount++;
      countlock.unlock();

      // Update state variables.
      gIncrement = true;
      gPrint = false;

      // Notify count complete.
      gOnCount.notify_all();

      // Wait for printing before counting next.
      printlock.lock();
      while (!gPrint)
         gOnPrint.wait(printlock);
      printlock.unlock();
   };

   t.join();
   return 0;
}
