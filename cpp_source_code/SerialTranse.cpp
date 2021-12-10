#include "SerialTranse.h"


SerialTranse::SerialTranse(const char path[]){
    int    ndx;
    int    cnt;
    char   buf[1024];
    struct termios    newtio;
    struct pollfd     poll_events;      // struct which has infomation to check
    int    poll_state;

    int    printed = 0;

    // port open
    this->fd_ = open(path, O_RDWR | O_NOCTTY );//"/dev/ttyUSB0"
    
    // configure serial port
    memset(&newtio, 0, sizeof(newtio) );
    newtio.c_cflag       = B115200  | CS8 | CLOCAL | CREAD;
    newtio.c_oflag       = 0;
    newtio.c_lflag       = 0;
    newtio.c_cc[VTIME]   = 0;
    newtio.c_cc[VMIN]    = 1;

    tcflush(this->fd_, TCIFLUSH);
    tcsetattr(this->fd_, TCSANOW, &newtio);
    fcntl(this->fd_, F_SETFL, FNDELAY); 
}
SerialTranse::~SerialTranse(){
	close(this->fd_);
}

ssize_t SerialTranse::Write(const char* msg, size_t bytes){
	return write(this->fd_, msg, bytes); 	
}

ssize_t SerialTranse::Read(char* buffer, size_t bytes){
	bzero(buffer, bytes);
    size_t bytes_read = read(this->fd_, buffer, bytes);
    buffer[bytes_read] = '\0';
    return bytes_read;
}

