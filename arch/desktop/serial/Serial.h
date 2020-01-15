#ifndef _SERIAL_H_
#define _SERIAL_H_ 1

// #ifndef ssize_t
//   typedef ssize_t long
// #endif

    int set_interface_attribs (int fd, int speed, int parity);
    void set_blocking (int fd, int should_block);
    void uart_writestr(const char* string);
    void uart_write(void* data, size_t len);
    size_t uart_read(void* buffer, size_t charsToRead);
    int uart_open(const char* port, int baud, int blocking);


  class Serial {

      private:
		void setBaud(int speed);
		int readSerial();
		void putchar(char *c, int n);
		void putchar(char c);
		unsigned char* getBuff();

      public:
        Serial(const char *portname, int speed=115200);
        ~Serial();

        void writestr(const char* string);
        void write(void* data, size_t len);
        void write(uint8_t ch);
        size_t read(void* buffer, size_t charsToRead);
        
        void close();
  };


#endif