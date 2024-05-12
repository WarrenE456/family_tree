#ifndef IO_H
#define IO_H

#include <string.h>
#include <termio.h>
#include <unistd.h>

void clearScreen()
{
  const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
  write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);
}

struct termios termios_orig;

static void rawBegin(void)
{
    tcgetattr(STDIN_FILENO, &termios_orig);
    struct termios raw;
    memcpy(&raw, &termios_orig, sizeof(raw));
    raw.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
    raw.c_oflag &= ~OPOST;
    raw.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
    raw.c_cflag &= ~(CSIZE|PARENB);
    raw.c_cflag |= CS8;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

static void rawEnd(void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &termios_orig);
}

#endif
