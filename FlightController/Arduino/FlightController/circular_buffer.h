#ifndef _CIRCULAR_BUFFER_h
#define _CIRCULAR_BUFFER_h

class CircularBuffer
{
private:
  int size;
	char buffer[50];
  int read_index;
  int write_index;

public:

	CircularBuffer()
  {
    this->size = 50;
    this->read_index = 0;
    this->write_index = 0;
  };
	char read()
  {
    if (length() > 0)
    {
      char c = buffer[read_index];
      read_index = increment(read_index);
      return c;
    }
    return '\0';
  };

  bool write(char c)
  {
    /*
    Writes one character to the buffer. This will always succeed.
    Returns true in case of overflow, false otherwise. In case of overflow,
    any unread data will be overwritten.
    */
    bool overflow = false;
    if (length() == size - 1)
    {
      // Buffer has overflown and previous data will be overwritten.
      overflow = true;
      write_index = read_index;
    }
    buffer[write_index] = c;
    write_index = increment(write_index);
    return overflow;
  };

  void reset()
  {
    read_index = 0;
    write_index = 0;
  };

  int length()
  {
    int r = read_index;
    int w = write_index;
    if (r > w)
    {
      w += size;
    }
    return w-r;
  };

  int increment(int value)
  {
    value++;
    if (value > size - 1)
    {
      value -= size;
    }
    return value;
  };
};


#endif
