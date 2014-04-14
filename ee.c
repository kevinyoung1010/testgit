#include <stdio.h>
#include <linux/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

int main(int argc, char **argv)
{
    struct i2c_rdwr_ioctl_data e2prom_data;
    unsigned int fd;
    unsigned char slave_address, reg_address,value; 
    int ret;
    
    unsigned char buf0[10],buf1[10];
    
    fd = open("/dev/i2c/0", O_RDWR);
    
    if (!fd){
        printf("Error on opening the device file\n");
        return 0;
    }

    slave_address= 0x38;
    reg_address = 0;;
    value = 0xaa;
   
    e2prom_data .nmsgs = 2;
    e2prom_data .msgs = (struct i2c_msg *)malloc(e2prom_data.nmsgs * sizeof(struct i2c_msg));


    if (!e2prom_data.msgs){
        printf("Memory alloc error\n");
        close(fd);
        return 0;
    }
    
    ioctl(fd, I2C_TIMEOUT, 2);
    ioctl(fd, I2C_RETRIES, 1);
    if (argc == 3)
    {
            if (argv[1][0]=='-' && argv[1][1]=='r')
            {
                sscanf(argv[2],"%x",&reg_address);
                /*read data from e2prom*/
                e2prom_data.nmsgs = 2;
                e2prom_data.msgs[0].buf = buf0;

                e2prom_data.msgs[0].len = 1;
                e2prom_data.msgs[0].addr = slave_address; 
                e2prom_data.msgs[0].flags = 0;
                e2prom_data.msgs[0].buf[0] = reg_address;
                    
                e2prom_data.msgs[1].len = 1;
                e2prom_data.msgs[1].addr = slave_address;
                e2prom_data.msgs[1].flags = I2C_M_RD;
                e2prom_data.msgs[1].buf = buf1;
                e2prom_data.msgs[1].buf[0] = 0;
                ret = ioctl (fd, I2C_RDWR, (unsigned long)&e2prom_data);
                if (ret < 0){
                    printf ("ioctl read error\n");
                }

                printf("read %02x,from dvi address %02x\n",buf1[0], reg_address);
                            
            }
    }

    if (argc == 4)
    {
        if (argv[1][0]=='-' && argv[1][1]=='w')
        {
            sscanf(argv[2],"%x",&reg_address);
            sscanf(argv[3],"%x",&value);
            value = atoi(argv[1]);
         

            /* write data to e2prom*/
            e2prom_data.nmsgs = 1;  
            e2prom_data.msgs[0].len = 4;

            e2prom_data.msgs[0].addr = slave_address;
            e2prom_data.msgs[0].flags = 0; 
            e2prom_data.msgs[0].buf = buf0;
            buf0[0] = reg_address;
            buf0[1] = value;
            buf0[2] = 0x08;
            buf0[3] = 0x10;
            
            ret = ioctl (fd, I2C_RDWR, (unsigned long)&e2prom_data);
            if (ret < 0){
                printf ("ioctl write error\n");
            }

            printf("you have write %02x into e2prom at %02x address\n",value,reg_address);
        }
       
        /*read data from e2prom*/
        e2prom_data.nmsgs = 2;
        e2prom_data.msgs[0].buf = buf0;

        e2prom_data.msgs[0].len = 1;
        e2prom_data.msgs[0].addr = slave_address; 
        e2prom_data.msgs[0].flags = 0;
        e2prom_data.msgs[0].buf[0] = 0x8;
            
        e2prom_data.msgs[1].len = 3;
        e2prom_data.msgs[1].addr = slave_address;
        e2prom_data.msgs[1].flags = I2C_M_RD;
        e2prom_data.msgs[1].buf = buf1;
        e2prom_data.msgs[1].buf[0] = 0;
        ret = ioctl (fd, I2C_RDWR, (unsigned long)&e2prom_data);
        if (ret < 0){
            printf ("ioctl read error\n");
        }
        sleep(1);
        printf("read %02x,%02x,%02x,%02x from e2prom address %02x\n",buf1[0],buf1[1],buf1[2],buf1[3], reg_address); 
    }
    close(fd);
    return 0;    
}

