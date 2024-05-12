#define _GNU_SOURCE // gettid()
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
#include <limits.h>
#include <sys/wait.h>

void sighandler(int sig, siginfo_t* info, void* _)
{
  write(2, "---------\n", 10);
  write(2, "sighandler\n", 11);
  const char* str = "Unknown";
  switch (info->si_code) {
#ifdef SI_DETHREAD
  case SI_DETHREAD: str = "SI_DETHREAD"; break;
#endif
#ifdef SI_TIMER
  case SI_TIMER: str = "SI_TIMER"; break;
#endif
#ifdef SI_USER
  case SI_USER: str = "SI_USER"; break;
#endif
#ifdef SI_QUEUE
  case SI_QUEUE: str = "SI_QUEUE"; break;
#endif
#ifdef SI_SIGIO
  case SI_SIGIO: str = "SI_SIGIO"; break;
#endif
#ifdef SI_MESGQ
  case SI_MESGQ: str = "SI_MESGQ"; break;
#endif
#ifdef SI_TKILL
  case SI_TKILL: str = "SI_TKILL"; break;
#endif
#ifdef SI_ASYNCIO
  case SI_ASYNCIO: str = "SI_ASYNCIO"; break;
#endif
#ifdef SI_ASYNCNL
  case SI_ASYNCNL: str = "SI_ASYNCNL"; break;
#endif
#ifdef SI_KERNEL
  case SI_KERNEL: str = "SI_KERNEL"; break;
#endif
#ifdef SI_NOINFO
  case SI_NOINFO: str = "SI_NOINFO"; break;
#endif
#ifdef SI_LWP
  case SI_LWP: str = "SI_LWP"; break;
#endif
#ifdef CLD_CONTINUED
  case CLD_CONTINUED: str = "CLD_CONTINUED"; break;
#endif
#ifdef CLD_DUMPED
  case CLD_DUMPED: str = "CLD_DUMPED"; break;
#endif
#ifdef CLD_EXITED
  case CLD_EXITED: str = "CLD_EXITED"; break;
#endif
#ifdef CLD_KILLED
  case CLD_KILLED: str = "CLD_KILLED"; break;
#endif
#ifdef CLD_NOOP
  case CLD_NOOP: str = "CLD_NOOP"; break;
#endif
#ifdef CLD_STOPPED
  case CLD_STOPPED: str = "CLD_STOPPED"; break;
#endif
#ifdef CLD_TRAPPED
  case CLD_TRAPPED: str = "CLD_TRAPPED"; break;
#endif
  }
  write(2, "si_code = ", 10);
  write(2, str, strlen(str));
  write(2, "\n", 1);
  char buf[20];
  write(2, "si_signo = ", 11);
  snprintf(buf, sizeof buf, "%d", info->si_signo);
  write(2, buf, strlen(buf));
  write(2, "\n", 1);
  write(2, "si_code = ", 10);
  snprintf(buf, sizeof buf, "%d", info->si_code);
  write(2, buf, strlen(buf));
  write(2, " (0x", 4);
  snprintf(buf, sizeof buf, "%x", info->si_code);
  write(2, buf, strlen(buf));
  write(2, ")\n", 2);
  write(2, "si_pid = ", 9);
  snprintf(buf, sizeof buf, "%d", info->si_pid);
  write(2, buf, strlen(buf));
  write(2, "\n", 1);
  if (sig == SIGCHLD) {
    write(2, "si_status = ", 12);
    snprintf(buf, sizeof buf, "%d", info->si_status);
    write(2, buf, strlen(buf));
    write(2, "\n", 1);
  }
}
int main()
{
  printf("pid = %d\n", getpid());
  printf("ppid = %d\n\n", getppid());

  struct sigaction act;
  act.sa_flags = SA_SIGINFO;
  sigemptyset(&act.sa_mask);
  act.sa_sigaction = sighandler;
  if (sigaction(SIGINT, &act, NULL) == -1) {
    perror("sigaction");
    return 1;
  }
  if (sigaction(SIGCHLD, &act, NULL) == -1) {
    perror("sigaction");
    return 1;
  }
  raise(SIGINT);
  kill(getpid(), SIGINT);
#if defined(__linux__) || defined(__FreeBSD__) || defined(__NetBSD__)
  union sigval _ = {0};
  sigqueue(getpid(), SIGINT, _);
#endif // linux || freebsd || netbsd
#if defined(__linux__) || defined(__NetBSD__)
  siginfo_t info;
  info.si_code = SI_TIMER;
  info.si_pid = 666;
# ifdef __linux__
  if (syscall(SYS_rt_sigqueueinfo, getpid(), SIGINT, &info) == -1)
    perror("rt_sigqueueinfo");
  if (syscall(SYS_rt_tgsigqueueinfo, getpid(), gettid(), SIGINT, &info))
    perror("rt_tgsigqueueinfo");
# else
  sigqueueinfo(getpid(), &info);
# endif
#endif // linux || netbsd
  if (!fork())_exit(1);
  wait(NULL);
  if (!fork())raise(SIGTERM);
  wait(NULL);
  sleep(1);
  puts("\n\nЯ ПОНИКАЮ НАХУЙ КОГДА ВОКРУГ ПАНННИКА\nТехХХХниКа\nМазаЙиККа\nkkAFF КАК-НииI-kaK.");
  sleep(-1u);
}
