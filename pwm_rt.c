#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <gpiod.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdint.h>
#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <sys/mman.h>

char buf[5], str[10];
uint32_t T = 900000000, c = 0, r = 0;
int fd;
int dfile;
char kbhit(void);
timer_t timerid;

const char *chipname="gpiochip1";
struct sigevent sev;
struct itimerspec trigger;
struct gpiod_chip *chip;
struct gpiod_line *lineRed;
int i;
int rt_init(void);
int ret;
pthread_attr_t attr;
pthread_cond_t cv, pwm;
pthread_mutex_t lock;

/* compile with gcc -pthread lockwait.c */
void Tstimer_thread(union sigval sv)
{
gpiod_line_set_value(lineRed, (i & 1) != 0);
i++;
//puts("100ms elapsed.");

pthread_mutex_lock(&lock);
pthread_cond_signal(&cv);
pthread_mutex_unlock(&lock);

timer_settime(timerid, 0, &trigger, NULL);
}

void *thread_adc(void *v)
{
for(;;)
{
pthread_mutex_lock(&lock);
pthread_cond_wait(&cv,&lock);
pthread_mutex_unlock(&lock);

//fd=open("adc/iio:device0/in_voltage3_raw",O_RDONLY);
//fd=open("/sys/bus/iio/devices/iio:device0/in_voltage3_raw",O_RDONLY);
fd=open("/sys/bus/iio/devices/iio:device0/in_voltage3_raw",O_RDONLY);
read(fd,buf,5);
//printf("value : %d \n",atoi(buf));
printf("value : %d \n",atoi(buf));
close(fd);

pthread_mutex_lock(&lock);
pthread_cond_signal(&pwm);
pthread_mutex_unlock(&lock);
}
return NULL;
}



void *thread_pwm(void *v)
{
for(;;)
{
pthread_mutex_lock(&lock);
pthread_cond_wait(&pwm,&lock);
pthread_mutex_unlock(&lock);
uint32_t c = atoi(buf);

r = T/5 + ((T*c)/5>>10);
sprintf(str, "%d", r);

//int dfile = open("PWM1A/duty_cycle", O_WRONLY);
//int dfile = open("/sys/class/pwm/pwmchip1/pwm0/duty_cycle", O_WRONLY);
     int dfile = open("/sys/class/pwm/pwmchip1/pwm0/duty_cycle", O_WRONLY);
write(dfile, str, sizeof(str));
close(dfile);
sleep(1);
}
return NULL;
}
void init_gpio(void)
{
chip = gpiod_chip_open_by_name(chipname);
lineRed = gpiod_chip_get_line(chip,18);  
gpiod_line_request_output(lineRed, "led" , 0);
}

void init_pwm(void)
{
char str[20];
int dfile;
//pinmux

dfile = open("/sys/devices/platform/ocp/ocp:P9_16_pinmux/state", O_WRONLY);
write(dfile, "pwm", sizeof("pwm"));
close(dfile);


dfile = open("/sys/class/pwm/pwmchip1/pwm0/period", O_WRONLY);
sprintf(str, "%d", T);
write(dfile, str, sizeof(str));
close(dfile);

dfile = open("/sys/class/pwm/pwmchip1/pwm0/duty_cycle", O_WRONLY);
sprintf(str, "%d", T/5);
write(dfile, str, sizeof(str));
close(dfile);

dfile = open("/sys/class/pwm/pwmchip1/pwm0/enable", O_WRONLY);
write(dfile, "1", sizeof("1"));
close(dfile);
}




void init_timer(void)
{
memset(&sev, 0, sizeof(struct sigevent));
memset(&trigger, 0, sizeof(struct itimerspec));
int rt_init(void);
sev.sigev_notify = SIGEV_THREAD;
sev.sigev_notify_function = &Tstimer_thread;

timer_create(CLOCK_REALTIME, &sev, &timerid);
trigger.it_value.tv_sec = 0;
trigger.it_value.tv_nsec = 100000000; // 100ms
timer_settime(timerid, 0, &trigger, NULL);
}

int main (int argc, char** argv)
{
pthread_t adc_thread, pwm_thread ;
         
        if(rt_init()) exit(0);
pwm_thread = (pthread_t ) malloc(sizeof(pthread_t));
adc_thread = (pthread_t ) malloc(sizeof(pthread_t));

pthread_mutex_init(&lock, NULL);
pthread_cond_init(&cv, NULL);

init_timer();
init_pwm();
init_gpio();

//create threads
pthread_create(&adc_thread, NULL, &thread_adc, NULL);
pthread_create(&pwm_thread, NULL, &thread_pwm, NULL);

while (kbhit() != 'q');

pthread_cancel(adc_thread);
pthread_cancel(pwm_thread);
timer_delete(timerid);
gpiod_line_release(lineRed);
gpiod_chip_close(chip);


return 0;
}

int rt_init(void)
{
     
        struct sched_param param;
     
        /* Lock memory */
        if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
                printf("mlockall failed: %m\n");
                exit(-2);
        }
 
        /* Initialize pthread attributes (default values) */
        ret = pthread_attr_init(&attr);
        if (ret) printf("init pthread attributes failed\n");
 
        /* Set a specific stack size  */
        ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
        if (ret) printf("pthread setstacksize failed\n");
           
        /* Set scheduler policy and priority of pthread */
        ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
        if (ret) printf("pthread setschedpolicy failed\n");
             
        param.sched_priority = 80;
        ret = pthread_attr_setschedparam(&attr, &param);
        if (ret) printf("pthread setschedparam failed\n");
               
        /* Use scheduling parameters of attr */
        ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        if (ret) printf("pthread setinheritsched failed\n");
       
        return ret;
       
}
