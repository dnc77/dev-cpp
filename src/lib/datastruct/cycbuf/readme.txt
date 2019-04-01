cycbuf

Summary:
This is a cyclic buffer of various sizes and allows bulk read/writes to it.
The buffer can be read from and written to directly through the use of direct
access functions or indirectly by copying from/to provided buffers.

How it works:
The cyclic buffer is managed by an array of bytes. This array has a start,
end, head and tail pointers. The following established rules are defined to
ascertain consistency within the buffer behaviour:

* Buffer consists of a start and end defining the space of the whole buffer.
* Buffer has a head which defines the start of data
* Buffer has a tail which defines the end of data
* The tail pointer never contains any valid data
* The end pointer never contains any valid data
* When head == tail, the buffer is empty
* When head == tail - 1, the buffer is full
* When head == start and tail = end, buffer is also full

The cyclic buffer provides two sets of functions:
Copy functions are used to copy from/to existing buffers.
Direct access functions allow for providing the buffer directly to other
functions for direct access.

Direct access functions may be more efficient because there is no extra copying
to an additional buffer. If an additional buffer exists, using the copy
functions is recommended as they are easier to use (less mistakes).

The buffer reads data from the head (when it's available) and also writes to
the tail (when space behind the tail exists). The rules above explain the
internal behaviour of the cyclic buffer.

How to use:
1: Create a cycbuf<size> (one of the defined sizes in cycsiz) object.
2: To read into an external buffer, readcopy can be used.
3: To copy an external buffer to the cyclic buffer, writecopy can be used.
4: toString() converts the buffer to a readable string.

The direct access functions operate under some basic rules:

To read from the buffer, first a call to getReadHead() should be made to find
the number of bytes available and the pointer. THe returned pointer (const) can
be read and processed. After said number of bytes (or less) have been processed,
a call to pushHead() with the size should be made to move the head pointer so
that more space is made in the buffer for further writes.

To write to the buffer, a call is made to getWriteTail() first. This returns
a pointer to the internal buffer and a maximum size to write to it. Once
writing is complete, pushWriteTail() can be called with the number of bytes
(less than the returned maximum) written.

The cyclic buffer has in built error checking to avoid reading/writing more
than is allowed.

Thanks

Duncan Camilleri
