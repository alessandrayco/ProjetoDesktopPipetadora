#include "mbed.h" //incluindo a biblioteca mbed
#include "TextLCD.h" //incluindo a biblioteca do display LCD

//DECLARANDO PINOS IO ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
DigitalOut EN(PC_8); //definindo o pino IO do sinal EN
DigitalOut CW_Z(PC_6); //definindo o pino IO do sinal CW
DigitalOut CLK(PC_9); //definindo o pino IO do sinal CLK
DigitalOut LED(PA_5);
AnalogIn yzAxis(A1); //define o pino IO que lerá o joystick
InterruptIn be(D7); // era PB_9
InterruptIn chfdcZP(D4);
InterruptIn chfdcZN(D5);
TextLCD LCD(D8, D9, D4, D5, D6, D7); //definindo os pinos IO para o LCD

//DECLARANDO VARIÁVEIS GLOBAIS ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int z, conta_pulso;
float distancia_mm;
int estado_fdcZP=0, estado_fdcZN=0;
int estado_EM =0;

//DECLARANDO FUNÇÕES BASE ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------ROTINA DE EMERGENCIA
void emergencia_on()
{
    estado_EM=1;
    if(estado_EM==1){
    printf("\n\rBotao de emergencia ativado"); 
    }
}

void emergencia_off()
{
    estado_EM=0;
    if(estado_EM==0){
    printf("\n\rTchau"); 
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------ROTINA FDC EIXO Z

void chave_fdc_ZP_on(){
    estado_fdcZP=0;
    estado_fdcZN=1;
}

void chave_fdc_ZP_off(){
    estado_fdcZP=1;
}

void chave_fdc_ZN_on(){
    estado_fdcZP=1;
    estado_fdcZN=0;
}

void chave_fdc_ZN_off(){
    estado_fdcZN=1;
}

void check_chave_fdc_ZP(){
    if (estado_fdcZP==0) {
        EN=1;
    }
    else{
        EN=0;
    }
}

void check_chave_fdc_ZN(){
    if (estado_fdcZN==0) {
        EN=1;
    }
    else{
        EN=0;
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------ROTINAS JOG
void jog_manual(){
    z = yzAxis.read() * 1000;
    if (z>550) { 
            CW_Z=0;
            check_chave_fdc_ZP();
        }
        if (z<550 && z>400){
            z = yzAxis.read() * 1000;
            EN = 1;
        }
        if (z<400) {
            CW_Z=1;
            check_chave_fdc_ZN();
        }
}

//--------------------------------------------------------------------------------------------------------------------------------------CLK
void clk(){
    CLK = 1;
    wait_ms(1);
    CLK = 0;
    wait_ms(1);

    if (z>550 && EN==0) { 
        conta_pulso++;
        distancia_mm=conta_pulso*5/200;
    }
    if (z<550 && z>400){
        z = yzAxis.read() * 1000;
    }
    if (z<400) {
        conta_pulso--;
        distancia_mm=conta_pulso*5/200;
    }
}
//--------------------------------------------------------------------------------------------------------------------------------------IHM
void IHM_contador_passos(){
    LCD.printf("Numero de pulso:");
    LCD.printf(conta_pulso);

    LCD.printf("Distancia:");
    LCD.printf(distancia_mm);
}
//--------------------------------------------------------------------------------------------------------------------------------------FUNÇÃO PRINCIPAL
int main()
{    
    chfdcZP.fall(&chave_fdc_ZP_on);  
    chfdcZP.rise(&chave_fdc_ZP_off); 
    chfdcZN.fall(&chave_fdc_ZN_on);  
    chfdcZN.rise(&chave_fdc_ZN_off); 

    be.fall(&emergencia_on);
    be.rise(&emergencia_off);
    
    while(1){
        while(estado_EM==0){   
            clk();
            jog_manual(); 
            IHM_contador_passos();           
        }
    }      
}