#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

int shm_fd;
void* ptr;

const char* mem_name = "/upp_prodcons";
const int mem_size = 4096;

void signal_handler(int signum) {
    if (signum == SIGINT) {
        printf("Prijaty SIGINT - ukoncuji\n");
        munmap(ptr, mem_size);
        close(shm_fd);
        exit(0);
    }
}

int main() {
    srand(time(NULL));
    signal(SIGINT, signal_handler);

    shm_fd = shm_open(mem_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        printf("Nelze vytvorit sdilenou pamet\n");
        return 1;
    }

    if (ftruncate(shm_fd, mem_size) == -1) {
        printf("Nelze nastavit velikost sdilene pameti\n");
        return 1;
    }

    ptr = mmap(0, mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        printf("Nelze namapovat sdilenou pamet\n");
        return 1;
    }

    // TODO: vytvorte semafory pro synchronizaci

    while (1) {
        char message[64];
        sprintf(message, "Cislo: %d", rand() % 1000);
        memcpy(ptr, message, strlen(message) + 1);

        printf("Odeslano: %s\n", message);

		// TODO: notifikujte semafor konzumenta
        // TODO: misto sleepu pockejte na semafor od konzumenta
        usleep((500 + rand() % 1000) * 1000);
    }

    munmap(ptr, mem_size);
    close(shm_fd);

    return 0;
}
