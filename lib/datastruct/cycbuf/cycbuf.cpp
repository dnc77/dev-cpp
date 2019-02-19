// Date:    05th February 2019
// Purpose: A cyclic buffer of a defined size
//
// Version control
// 05 Feb 2019 Duncan Camilleri           Initial development
// 

// Includes
#include <sys/time.h>
#include <memory.h>
#include <string>
#include <cycbuf.h>

using namespace std;

template class cycbuf<tiny>;
template class cycbuf<small>;
template class cycbuf<medium>;
template class cycbuf<large>;
template class cycbuf<huge>;

//
// CONSTRUCTION/DESTRUCTION
//

template <unsigned int size>
cycbuf<size>::cycbuf()
{
   memset(mBuf, 0, size);
}

template <unsigned int size>
cycbuf<size>::~cycbuf()
{
}

//
// CONVERSION
//

// Returns a string representing the buffer.
template <unsigned int size>
std::string cycbuf<size>::toString()
{
   string buf;
   int n = 0;
   for (; n < size; ++n) {
      if (((char)mBuf[n]) == 0) buf += '.';
      else buf += (char)mBuf[n];
   }

   return buf;
}

//
// COPY FUNCTIONS
//

// Read copy will copy from the cyclic buffer into the pointer pBuf
// up to a length of s. The function will return with the number of
// bytes copied.
template <unsigned int size>
size_t cycbuf<size>::readcopy(byte* pBuf, size_t s)
{
   // Check for data availability and head at end of buffer.
   if (!isReadReady()) return 0;

   // Determine number of bytes to copy based on available data.
   // Remembering that tail always sits in an empty space which is not used.
   size_t avail = (mpHead > mpTail) ? mpEnd - mpHead : mpTail - mpHead;
   size_t copyBytes = min(avail, s);

   // Copy the memory to the cyclic buffer and empty.
   memcpy(pBuf, mpHead, copyBytes);
   memset(mpHead, 0, copyBytes);

   // Move the head.
   mpHead += copyBytes;

   // If no more data to copy, succeed.
   size_t remaining = (s == copyBytes ? 0 : s - copyBytes);
   if (remaining == 0) return copyBytes;

   // More data needs to be copied.
   return copyBytes + readcopy(pBuf + copyBytes, remaining);
}

// Write copy will copy the buffer pBuf into the cyclic buffer.
// The write function will attempt to write as many bytes as possible.
// The function will return with the number of bytes copied.
template <unsigned int size>
size_t cycbuf<size>::writecopy(byte* pBuf, size_t s)
{
   // Check for space availability and tail at end of buffer.
   if (!isWriteReady()) return 0;

   // Determine number of bytes to copy based on available space.
   // Remembering that tail always sits in an empty space which is not used.
   size_t avail = (mpHead > mpTail) ? mpHead - (mpTail + 1) : mpEnd - mpTail;
   size_t copyBytes = min(avail, s);

   // Copy the memory to the cyclic buffer.
   memcpy(mpTail, pBuf, copyBytes);

   // Move the tail.
   mpTail += copyBytes;

   // If no more data to copy, succeed.
   size_t remaining = (s == copyBytes ? 0 : s - copyBytes);
   if (remaining == 0) return copyBytes;

   // More data needs to be copied.
   return copyBytes + writecopy(pBuf + copyBytes, remaining);
}

//
// DIRECT ACCESS FUNCTIONS
// These functions provide direct access to the cyclic buffer
// memory. Using direct access functions allow for more speed as
// they reduce the need to copy buffers however their use comes at
// a cost in that they require more caution.
// Call getReadHead to get a function pointer to the data in the
// cyclic buffer and also a size to specify how much data is available.
// Call pushHead and the size of bytes that is no longer necessary. Be
// wary that the size of bytes to push is within the constraints of the
// state of the buffer. getReadHead provides the maximum at the time of
// it's call.
// Likewise, getWriteTail gives access to a buffer and a size which can be
// used to write data to. This data will be written to the cyclic buffer.
// After writing, a call to pushTail with the number of bytes written should
// be made. Same conditions as the read functions apply in terms of the size.
//

// Returns a read only buffer pointing to the start of the cyclic
// buffer. s will have the number of bytes that are available for
// reading in the cyclic buffer.
template <unsigned int size>
byte const* cycbuf<size>::getReadHead(size_t& s)
{
   // Check for data availability and head at end of buffer.
   if (!isReadReady()) {
      s = 0;
      return nullptr;
   }

   // Determine data available and return.
   s = (mpHead > mpTail) ? mpEnd - mpHead : mpTail - mpHead;
   return mpHead;
}

// Moves the head to the right, implying that data can be disposed
// from the old position of the head onwards.
template <unsigned int size>
void cycbuf<size>::pushHead(size_t s)
{
   // Check for data availability and head at end of buffer.
   if (!isReadReady()) return;

   // Determine data available and return.
   size_t avail = (mpHead > mpTail) ? mpEnd - mpHead : mpTail - mpHead;
   size_t dispose = min(s, avail);

   // Clear memory and move head.
   memset(mpHead, 0, dispose);
   mpHead += dispose;
}

// Returns a buffer where data can be stored and the size up to how much
// of that buffer can be written to.
template <unsigned int size>
byte* cycbuf<size>::getWriteTail(size_t& s)
{
   // Check for space availability and tail at end of buffer.
   if (!isWriteReady()) {
      s = 0;
      return nullptr;
   }

   // Determine number of bytes to copy based on available space.
   // Remembering that tail always sits in an empty space which is not used.
   s = (mpHead > mpTail) ? mpHead - (mpTail + 1) : mpEnd - mpTail;
   return mpTail;
}

template <unsigned int size>
void cycbuf<size>::pushWriteTail(size_t s)
{
   // Check for space availability and tail at end of buffer.
   if (!isWriteReady()) return;

   // Determine number of bytes to copy based on available space.
   // Remembering that tail always sits in an empty space which is not used.
   size_t avail = (mpHead > mpTail) ? mpHead - (mpTail + 1) : mpEnd - mpTail;
   size_t bypass = min(s, avail);

   // Move tail.
   mpTail += bypass;
}

//
// BUFFER STATUS CHECKS
//

// Checks whether the buffer is empty.
// A buffer is empty when the head and tail point to
// the same location. Since the end position is never
// used, if the head is at or after the end position,
// it's implied that the next starting location is
// the start of the buffer. However, if the tail (which is
// also never used) is at the start of the buffer, then it
// can be implied that both head and tail are in the same
// location. In that case, the buffer is also assumed to be empty.
template <unsigned int size>
inline bool cycbuf<size>::isEmpty()
{
   return mpHead == mpTail ||
      (mpHead >= mpEnd && mpTail == mpStart);
}

// Checks whether the buffer is full.
// This is when the tail is one position ahead of the head.
// Like isEmpty(), the end position of the buffer also needs to
// be taken into account.
template <unsigned int size>
inline bool cycbuf<size>::isFull()
{
      return (mpHead == mpStart && mpTail >= mpEnd) ||
         (mpTail == (mpHead - 1));
}

// Checks to make sure that there is data in the buffer (by calling
// isEmpty() and also adjusts head position for reading if it's at the
// end of the buffer.
template <unsigned int size>
inline bool cycbuf<size>::isReadReady()
{
   // Check for empty buffer first.
   if (isEmpty()) return false;

   // If head has reached past the end of the buffer, move it to
   // start. Data is always available as per check above (isEmpty()).
   if (mpHead >= mpEnd) mpHead = mpStart;
   return true;
}

// Ensures there is space for writing in the buffer. Also checks to
// see if the tail needs to be repositioned for writing whenever it is
// at the end of the buffer.
template <unsigned int size>
inline bool cycbuf<size>::isWriteReady()
{
   // Can never write to a full buffer.
   if (isFull()) return false;

   // If tail has reached the end of the buffer, move it to
   // start. Space is always available as per check above (isFull()).
   if (mpTail >= mpEnd) mpTail = mpStart;
   return true;
}
