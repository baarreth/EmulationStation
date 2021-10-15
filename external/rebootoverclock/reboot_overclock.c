#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    setuid(0);
    system("cp /boot/firmware/config.txt /boot/firmware/config.txt.bkp && cat /boot/firmware/config_overclock.txt >> /boot/firmware/config.txt && reboot");
    return 0;
}
