#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "Serial.h"

/* use omega UART1 */
// const char *portname = "/dev/ttyS1";
char *portname = NULL;
int uartFd = -1;
bool _uart_failed = false;

// ==========================================================
#define LG_BUFFER 1024

void Serial::writestr(const char *string)
{
	putchar((char *)string, strlen(string));
}
void Serial::write(void *data, size_t len)
{
	putchar((char *)data, len);
}

size_t Serial::read(void *buffer, size_t charsToRead)
{
	int read = readSerial();

	if (read <= 0)
	{
		return read;
	}
	memcpy(buffer, getBuff(), read);

	return read;
}

// #define DBUG_SERIAL 1

int nb_read;
unsigned char buffer[LG_BUFFER];
struct termios tio;
int tty_fd;

void setspeed(speed_t vitesse)
{
	cfsetospeed(&tio, vitesse);
	cfsetispeed(&tio, vitesse);
	tcsetattr(tty_fd, TCSANOW, &tio);
}

Serial::Serial(const char *acm = "/dev/ttyACM0", int vitesse /* = 9600*/)
{

	memset(&tio, 0, sizeof(tio));
	tio.c_iflag = 0; //IGNPAR ;
	tio.c_oflag = 0;
	tio.c_cflag = CS8 | CREAD | CLOCAL; // 8n1, see termios.h for more information
	tio.c_lflag = 0;
	tio.c_cc[VMIN] = 1;
	tio.c_cc[VTIME] = 5;

	tty_fd = open(acm, O_RDWR | O_NONBLOCK);
	if (tty_fd < 0)
	{
		perror("open");
		printf("file => %s\n", acm);
		// exit(EXIT_FAILURE) ;
	}
	else
	{
		this->setBaud(vitesse);
	}
}

void Serial::setBaud(int vitesse)
{

	if (vitesse < 51)
		setspeed(B50);
	else if (vitesse < 76)
		setspeed(B75);
	else if (vitesse < 111)
		setspeed(B110);
	else if (vitesse < 135)
		setspeed(B134);
	else if (vitesse < 151)
		setspeed(B150);
	else if (vitesse < 201)
		setspeed(B200);
	else if (vitesse < 301)
		setspeed(B300);
	else if (vitesse < 601)
		setspeed(B600);
	else if (vitesse < 1201)
		setspeed(B1200);
	else if (vitesse < 1801)
		setspeed(B1800);
	else if (vitesse < 2401)
		setspeed(B2400);
	else if (vitesse < 4801)
		setspeed(B4800);
	else if (vitesse < 9601)
		setspeed(B9600);
	else if (vitesse < 19201)
		setspeed(B19200);
	else if (vitesse < 34001)
		setspeed(B38400);
	else if (vitesse < 57601)
		setspeed(B57600);
	else
		setspeed(B115200);
}

void Serial::putchar(char c)
{
	::write(tty_fd, &c, 1);
}

void Serial::putchar(char *c, int n)
{
	::write(tty_fd, c, n);
}

void Serial::write(uint8_t c)
{
	this->putchar(c);
}

int Serial::readSerial()
{
	// en attendant la nouvelle version
	nb_read = ::read(tty_fd, buffer, LG_BUFFER);

#ifdef DBUG_SERIAL
	// putputs to stdout
	::write(STDOUT_FILENO, buffer, nb_read);
#endif
	return nb_read;
}

void Serial::close()
{
	if (::close(tty_fd) < 0)
		printf("err close serial\n");
	// exit(EXIT_FAILURE) ;
}

unsigned char *Serial::getBuff() { return buffer; }

// ==========================================================

// Serial::Serial(const char *_portname, int speed) {
//   portname = (char*)_portname;

//   int blocking = 0;
//   //blocking = 1;

//   if(! (uart_open(portname, speed == 115200 ? B115200 : B9600, blocking) > 0) ) {
// 	_uart_failed = true;
//         printf("FAILED TO OPEN 'Serial' !!! \n");
//   }
// }
Serial::~Serial()
{
}

// void Serial::writestr(const char* string) {
//   if ( _uart_failed ) { return; }
//   uart_writestr(string);
// }
// void Serial::write(void* data, size_t len) {
//   if ( _uart_failed ) { return; }
//   uart_write(data, len);
// }
// size_t Serial::read(void* buffer, size_t charsToRead) {
//   if ( _uart_failed ) { return 0; }
//   return uart_read(buffer, charsToRead);
// }

// ==============================================================================

// int set_interface_attribs (int fd, int speed, int parity)
// {
//         struct termios tty;
//         memset (&tty, 0, sizeof tty);
//         if (tcgetattr (fd, &tty) != 0)
//         {
//                 fprintf (stderr, "error %d from tcgetattr\n", errno);
//                 return -1;
//         }

//         cfsetospeed (&tty, speed);
//         cfsetispeed (&tty, speed);

//         tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
//         // disable IGNBRK for mismatched speed tests; otherwise receive break
//         // as \000 chars
//         tty.c_iflag &= ~IGNBRK;         // disable break processing
//         tty.c_lflag = 0;                // no signaling chars, no echo,
//                                         // no canonical processing
//         tty.c_oflag = 0;                // no remapping, no delays

//         tty.c_cc[VMIN]  = 0;            // read doesn't block

//         // tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout
//         tty.c_cc[VTIME] = 2;            // 0.2 seconds read timeout

//         tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

//         tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
//                                         // enable reading
//         tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
//         tty.c_cflag |= parity;
//         tty.c_cflag &= ~CSTOPB;
//         tty.c_cflag &= ~CRTSCTS;

//         if (tcsetattr (fd, TCSANOW, &tty) != 0)
//         {
//                 fprintf (stderr, "error %d from tcsetattr", errno);
//                 return -1;
//         }
//         return 0;
// }

// void set_blocking (int fd, int should_block)
// {
//         struct termios tty;
//         memset (&tty, 0, sizeof tty);
//         if (tcgetattr (fd, &tty) != 0)
//         {
//                 fprintf (stderr, "error %d from tggetattr", errno);
//                 return;
//         }

//         tty.c_cc[VMIN]  = should_block ? 1 : 0;
//         tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

//         if (tcsetattr (fd, TCSANOW, &tty) != 0)
//                 fprintf (stderr, "error %d setting term attributes", errno);
// }

// void uart_writestr(const char* string) {
// 	write(uartFd, string, strlen(string));
// }

// void uart_write(void* data, size_t len) {
// 	write(uartFd, data, len);
// }

// size_t uart_read(void* buffer, size_t charsToRead) {
// 	return read(uartFd, buffer, charsToRead);
// }

// int uart_open(const char* port, int baud, int blocking) {
// 	uartFd = open (port, O_RDWR | O_NOCTTY | O_SYNC);
// 	if (uartFd < 0)
// 	{
// 			fprintf (stderr, "error %d opening %s: %s", errno, port, strerror (errno));
// 			return -1;
// 	}
// 	int ok = set_interface_attribs (uartFd, baud, 0);  // set speed, 8n1 (no parity)
//         if ( ok < 0 ) { return -1; }
// 	set_blocking (uartFd, blocking); //set blocking mode
// 	printf("Port %s opened.\n", port);
// 	return 1;
// }
