#ifndef _CIRCULAR_BUFFER_h
#define _CIRCULAR_BUFFER_h

class CircularBuffer
{
private:
  int size = 100;
	char buffer[100];
  int read_index = 0;
  int write_index = 0;

public:

	CircularBuffer();
	char read()
  {
    if (length() > 0)
    {
      char c = buffer[read_index];
      read_index = increment(read_index);
      return c;
    }
    return NULL;
  }

  bool write(char c)
  {
    /*
    Writes one character to the buffer. This will always succeed.
    In case of buffer overflow, the buffer will be effectively cleared and
    false is returned. Returns true otherwise.
    */
    bool no_overflow = true;
    if (length() == size - 1)
    {
      // Buffer has overflown and previous data will be overwritten.
      no_overflow = false;
      write_index = read_index;
    }
    buffer[write_index] = c;
    write_index = increment(write_index);
    return no_overflow;
  }

  int length()
  {
    int r = read_index;
    int w = write_index;
    if (r > w)
    {
      w += size;
    }
    return w-r;
  }

  int increment(int value)
  {
    value++;
    if (value > size - 1)
    {
      value -= size;
    }
    return value;
  }
};


#endif
