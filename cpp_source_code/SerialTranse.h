#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <termios.h>    // constants(B115200, CS8, etc) defined
#include <fcntl.h>      // constants(RDWR , O_NOCTTY, etc) defined

class SerialTranse {        
public:
	SerialTranse(const char path[]="/dev/ttyACM0");
    ~SerialTranse();
    ssize_t Write(const char* msg, size_t bytes);
    ssize_t Read(char* buffer, size_t bytes);
private:
    int fd_;
};
   
