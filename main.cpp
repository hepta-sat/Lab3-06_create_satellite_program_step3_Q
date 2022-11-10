#include "mbed.h"
#include "HEPTA_CDH.h"
#include "HEPTA_EPS.h"
HEPTA_CDH cdh(p5, p6, p7, p8, "sd");
HEPTA_EPS eps(p16,p26);
DigitalOut condition(LED1);
RawSerial gs(USBTX,USBRX,9600);
Timer sattime;
int rcmd = 0, cmdflag = 0; //command variable
 
//getting command and flag
void commandget()
{
    rcmd = gs.getc();
    cmdflag = 1;
}
//interrupting 
void receive(int rcmd, int cmdflag)
{
    gs.attach(commandget,Serial::RxIrq);
}
//initialzing
void initialize()
{
    rcmd = 0;
    cmdflag = 0;
    condition = 0;
}

int main() 
{
    gs.printf("From Sat : Operation Start...\r\n");
    int flag = 0;      // condition
    float batvol,temp; //Voltage, Temerature
    sattime.start();
    receive(rcmd,cmdflag); //interupting by ground station command
    eps.turn_on_regulator();//turn on 3.3V conveter
    for(int i = 0; i < 100; i++) {
        //satellite condition led
        condition = !condition;
        
        //senssing HK data(dummy data)
        eps.vol(&batvol);
        temp   = 28.5;
        
        //Transmitting HK data to Ground Station(GS)
        gs.printf("HEPTASAT::Condition = %d, Time = %f [s], batVol = %.2f [V],Temp = %.2f [C]\r\n",flag,sattime.read(),batvol,temp);
        wait_ms(1000);
        
        //Power Saving Mode 
        if(batvol <= 3.5){
            eps.shut_down_regulator();
            gs.printf("Power saving mode ON\r\n"); 
            flag = 1;
        } else if((flag == 1) & (batvol > 3.7)) {
            eps.turn_on_regulator();
            gs.printf("Power saving mode OFF\r\n");
            flag = 0;
        }
        
        if (cmdflag == 1) {
            if (rcmd == 'a') {
               for(int j=0;j<5;j++){
                gs.printf("HEPTASAT::Hello World!\r\n");
                condition = 1;
                wait_ms(1000);
               }
            }else if(rcmd == 'b') {
 
 
 

            }
            initialize();
        }
    }
    sattime.stop();
    gs.printf("From Sat : Operation Stop...\r\n");
}