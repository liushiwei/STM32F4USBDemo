#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include <sys/time.h>
#define STM32F4
#if defined STM32F1
# include <stm32f10x.h>
#elif defined STM32F4
# include <stm32f4xx.h>
#endif

#if defined STM32F4
uint64_t virtualClock = 0;
#endif

extern uint32_t __get_MSP(void);

#if defined STM32F1
# define STDOUT_USART USART1
# define STDERR_USART USART1
# define STDIN_USART USART1
#elif defined STM32F4
# define STDOUT_USART USART3
# define STDERR_USART USART3
# define STDIN_USART USART3
#endif

#undef errno
extern int errno;
extern UART_HandleTypeDef UartHandle;
char *__env[1] = { 0 };
char **environ = __env;

int _write(int file, char *ptr, int len);


int _close(int file)
{
    return -1;
}

int _execve(char *name, char **argv, char **env)
{
    errno = ENOMEM;
    return -1;
}

int _fork()
{
    errno = EAGAIN;
    return -1;
}

int _fstat(int file, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

int _getpid()
{
    return 1;
}

int _gettimeofday(struct timeval *tv, struct timezone *tz)
{
#if defined STM32F1
    tv->tv_sec = RTC_GetCounter();
    tv->tv_usec = 0;
#elif defined STM32F4
    tv->tv_sec = virtualClock / 1000;
    tv->tv_usec = (virtualClock % 1000) * 1000;
#endif
    return 0;
}

int _isatty(int file)
{
    switch (file)
    {
    case STDOUT_FILENO:
    case STDERR_FILENO:
    case STDIN_FILENO:
        return 1;
    default:
        //errno = ENOTTY;
        errno = EBADF;
        return 0;
    }
}

int _kill(int pid, int sig)
{
    errno = EINVAL;
    return (-1);
}

int _link(char *old, char *new)
{
    errno = EMLINK;
    return -1;
}

int _lseek(int file, int ptr, int dir)
{
    return 0;
}

int _read(int file, char *ptr, int len)
{
    switch (file)
    {
    case STDIN_FILENO:
    	HAL_UART_Receive_DMA(&UartHandle,ptr,len);
        return 1;
        break;
    default:
        errno = EBADF;
        return -1;
    }
}

int _stat(const char *filepath, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

clock_t _times(struct tms *buf)
{
    return -1;
}

int _unlink(char *name)
{
    errno = ENOENT;
    return -1;
}

int _wait(int *status)
{
    errno = ECHILD;
    return -1;
}

int _write(int file, char *ptr, int len)
{
    int n;
    switch (file)
    {
    case STDOUT_FILENO: /*stdout*/
//        for (n = 0; n < len; n++)
//        {
//            while (USART_GetFlagStatus(STDOUT_USART, USART_FLAG_TC) == RESET);
//            USART_SendData(STDOUT_USART, (uint8_t) * ptr++);
//        }
//    	HAL_UART_Transmit_DMA(&UartHandle, ptr, len);
        break;
    case STDERR_FILENO: /* stderr */
//    	HAL_UART_Transmit_DMA(&UartHandle, ptr, len);
        break;
    default:
        errno = EBADF;
        return -1;
    }
    return len;
}
