#include <sys/types.h>
#include <stddef.h>

/* ========================================================================= *
 * DECLARACIONES PREVIAS (Para evitar incluir cabeceras que no existen aún)
 * ========================================================================= */
struct termios;
struct pollfd;
struct sigaction;
struct rlimit;
struct tms;
struct timeval;
struct timezone;
struct passwd;
struct group;

typedef struct {
    int gl_pathc;
    char **gl_pathv;
    int gl_offs;
} glob_t;

/* ========================================================================= *
 * STUBS GRUPO B: Funciones avanzadas que Busybox pide pero que 
 * un SO bare-metal básico no necesita para arrancar una shell.
 * ========================================================================= */

/* 1. Terminal y Control de Flujo */
int tcgetattr(int fd, struct termios *termios_p) { return -1; }
int tcsetattr(int fd, int optional_actions, const struct termios *termios_p) { return -1; }
int isatty(int fd) { return 1; } /* Engañamos a la shell diciendo que todo es una terminal para que sea interactiva */

/* 2. Sincronización y Eventos */
int poll(struct pollfd *fds, unsigned int nfds, int timeout) { return -1; }

/* 3. Señales Avanzadas */
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) { return -1; }
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset) { return -1; }
int sigsuspend(const sigset_t *mask) { return -1; }

/* 4. Límites y Configuración del Sistema */
int getrlimit(int resource, struct rlimit *rlim) { return -1; }
int setrlimit(int resource, const struct rlimit *rlim) { return -1; }
long sysconf(int name) { return -1; }

/* 5. VFS Avanzado y Modificadores */
ssize_t readlink(const char *pathname, char *buf, size_t bufsiz) { return -1; }
mode_t umask(mode_t mask) { return 022; }
int fcntl(int fd, int cmd, ...) { return -1; }

/* 6. Usuarios y Grupos (Falsificados como root) */
struct passwd *getpwuid(uid_t uid) { return NULL; }
struct group *getgrgid(gid_t gid) { return NULL; }
struct passwd *getpwnam(const char *name) { return NULL; }
uid_t geteuid(void) { return 0; } /* Root */
gid_t getegid(void) { return 0; } /* Root */
uid_t getuid(void) { return 0; }
gid_t getgid(void) { return 0; }
pid_t getppid(void) { return 1; } /* Init */

/* 7. Búsqueda y Expansión de Patrones */
int fnmatch(const char *pattern, const char *string, int flags) { return 1; } /* 1 = FNM_NOMATCH */
int glob(const char *pattern, int flags, int (*errfunc)(const char *, int), glob_t *pglob) { return 3; } /* 3 = GLOB_NOMATCH */
void globfree(glob_t *pglob) { }

/* 8. Tiempos */
clock_t times(struct tms *buf) { return -1; }
int gettimeofday(struct timeval *tv, struct timezone *tz) { return -1; }

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stddef.h>

/* ========================================================================= *
 * GRUPO A (Parte 1): PUENTES A TUS SYSCALLS REALES
 * Conectamos el estándar POSIX (sin guion) a tus implementaciones (con guion)
 * ========================================================================= */

/* Declaramos las firmas de lo que ya tienes en tu código para que el compilador no se queje */
extern ssize_t _read(int fd, void *buf, size_t count);
extern ssize_t _write(int fd, const void *buf, size_t count);
extern int _open(const char *pathname, int flags, ...);
extern int _close(int fd);
extern int _execve(const char *pathname, char *const argv[], char *const envp[]);
extern pid_t _fork(void);
extern pid_t _waitpid(pid_t pid, int *wstatus, int options);
extern pid_t _wait(int *wstatus);
extern char *_getcwd(char *buf, size_t size);
extern int _chdir(const char *path);
extern int _dup(int oldfd);
extern int _dup2(int oldfd, int newfd);
extern int _pipe(int pipefd[2]);
extern int _access(const char *pathname, int mode);
extern void *_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
extern int _munmap(void *addr, size_t length);
extern int _kill(pid_t pid, int sig);
extern void _exit(int status);

/* Funciones puente (lo que pide Busybox -> lo que hace tu kernel) */
ssize_t read(int fd, void *buf, size_t count) { return _read(fd, buf, count); }
ssize_t write(int fd, const void *buf, size_t count) { return _write(fd, buf, count); }
int open(const char *pathname, int flags, ...) { return _open(pathname, flags, 0); }
int close(int fd) { return _close(fd); }
int execve(const char *pathname, char *const argv[], char *const envp[]) { return _execve(pathname, argv, envp); }
pid_t fork(void) { return _fork(); }
pid_t waitpid(pid_t pid, int *wstatus, int options) { return _waitpid(pid, wstatus, options); }
pid_t wait(int *wstatus) { return _wait(wstatus); }
char *getcwd(char *buf, size_t size) { return _getcwd(buf, size); }
int chdir(const char *path) { return _chdir(path); }
int dup(int oldfd) { return _dup(oldfd); }
int dup2(int oldfd, int newfd) { return _dup2(oldfd, newfd); }
int pipe(int pipefd[2]) { return _pipe(pipefd); }
int access(const char *pathname, int mode) { return _access(pathname, mode); }
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) { return _mmap(addr, length, prot, flags, fd, offset); }
int munmap(void *addr, size_t length) { return _munmap(addr, length); }
int kill(pid_t pid, int sig) { return _kill(pid, sig); }
void exit(int status) { _exit(status); while(1); }


/* ========================================================================= *
 * GRUPO A (Parte 2): STUBS TEMPORALES PARA SYSCALLS FALTANTES
 * Funciones vitales que aún no tienes implementadas pero que el enlazador exige.
 * Devuelven error por defecto hasta que programes la lógica real en el VFS/Kernel.
 * ========================================================================= */

/* Control de Procesos y Memoria */
int getpid(void) { return 1; } /* Temporal: fingimos ser el init process */
void *sbrk(intptr_t increment) { return (void *)-1; } /* Crítico para que malloc funcione en el futuro */

/* Metadatos de Archivos (Stat) */
int stat(const char *pathname, struct stat *statbuf) { return -1; }
int fstat(int fd, struct stat *statbuf) { return -1; }
int lstat(const char *pathname, struct stat *statbuf) { return -1; }

/* Modificadores de VFS */
int unlink(const char *pathname) { return -1; }
int rmdir(const char *pathname) { return -1; }
off_t lseek(int fd, off_t offset, int whence) { return (off_t)-1; }

/* Iteración de Directorios (Necesario para el comando 'ls') */
DIR *opendir(const char *name) { return NULL; }
struct dirent *readdir(DIR *dirp) { return NULL; }
int closedir(DIR *dirp) { return -1; }

