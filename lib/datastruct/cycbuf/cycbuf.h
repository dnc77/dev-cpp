// Date:    05th February 2019
// Purpose: A cyclic buffer of a defined size
//
// Version control
// 05 Feb 2019 Duncan Camilleri           Initial development
// 10 Feb 2019 Duncan Camilleri           isEmpty() and isFull() are now public
// 

#ifndef __CYCBUF_H__
#define __CYCBUF_H__

// Check for missing includes.
#if not defined _GLIBCXX_STRING
#error "cycbuf.h: missing include - string"
#endif

// c++17 not installed. remove if compiling with a full c++17 support.
enum class byte : unsigned char {};

// Different cyclic buffer sizes.
enum cycsiz : unsigned int {
   tiny = 16,
   small = 128,
   medium = 1024,
   large = 4096,
   huge = 165536   
};

// Cyclic buffer
// Rules:
// * Buffer consists of a start and end defining the space of the whole buffer.
// * Buffer has a head which defines the start of data
// * Buffer has a tail which defines the end of data
// * The tail pointer never contains any valid data
// * The end pointer never contains any valid data
// * When head == tail, the buffer is empty
// * When head == tail - 1, the buffer is full
// * When head == start and tail = end, buffer is also full

template <unsigned int size>
class cycbuf
{
public:
   // Construction/Destruction
   cycbuf();
   virtual ~cycbuf();

   // Conversion.
   std::string toString();

   // Copy functions.
   size_t readcopy(byte* pBuf, size_t s);
   size_t writecopy(byte* pBuf, size_t s);

   // Direct access functions.
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
   byte const* getReadHead(size_t& s);
   void pushHead(size_t s);
   byte* getWriteTail(size_t& s);
   void pushWriteTail(size_t s);

private:
   bool mFailWrite = false;               // when the buffer is not big enough

   byte* mpHead = mBuf;                   // head of data
   byte* mpTail = mpHead;                 // tail of data
   byte* mpStart = mBuf;                  // start of buffer
   byte* mpEnd = &mBuf[size - 1];         // end of buffer
   byte mBuf[(int)size];                  // whole buffer

public:
   // Buffer status checks
   bool isEmpty();
   bool isFull();

private:
   bool isReadReady();
   bool isWriteReady();
};

#endif      // __CYCBUF_H__
