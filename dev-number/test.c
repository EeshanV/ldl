#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>


int main(){
    int dev = open("/dev/Edevice", O_RDONLY);
    if(dev == -1){
        printf("could not open\n");
        return -1;
    }
    printf("Opened succssfully\n");
    close(dev);
    return 0;
}