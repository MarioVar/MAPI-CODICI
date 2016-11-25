#include<stm32f30x.h>
#include<stdio.h>
float ADC_val,ADC_valc;
int main()
//setup
{
  //Attivare bus clk per GPIOA e ADC12
  RCC->AHBENR|=1<<17;
  RCC->AHBENR|=1<<28;
  RCC->CFGR2|=1<<8;
  //Configurare PA0 come input analogico
  GPIOA->MODER|=3<<0;
  //Configurare clock per adc nei registri RCC_CFGR2 e ADC12_CCR
  ADC1_2->CCR|=1<<16;
  ADC1->CFGR&=~((unsigned int)0x00080000);//azzero il bit #13
  //Sequenza regolatore di tensione (ADVREGEN 01 --> 00 --> 10)
  ADC1->CR&=~((unsigned int)0x30000000);
  ADC1->CR|=1<<29;
  //Attesa fine sequenza (~10us)
  for(int i=0;i<1000;i++);
  //VERIFICO CHE ADCEN=0
  if(ADC1->CR & 0x1)
    ADC1->CR&=~((unsigned int)1);

  //Avvio calibrazione dell'ADC (ADCAL = 1)
  ADC1->CR|=(unsigned int) 0x80000000;
  //Attesa fine calibrazione
  while((ADC1->CR & 0x80000000 )==0x80000000);
  //Avvio ADC (ADEN=1)
  ADC1->CR|=1<<0;
  //Attesa fine avvio (ADRDY = 1)
  while(!(ADC1->ISR & 0x1));
  //Configurazione elementi sequenza (L=0) canale di ingresso e periodo di campionamento
  ADC1->SQR1&=~((unsigned int)0x0000000f);
  ADC1->SQR1|=1<<6;
  ADC1->SMPR1&=~((unsigned int)0x00000034);
 // LOOP
  while(1){
    //Inizia conversione (ADSTART = 1)
    if(GPIOA->IDR&0x1==0x1){//se il tasto è premuto allora converto!
      ADC1->CR|=1<<2;
      //Attesa fine conversione (EOC = 1)
       while(!(ADC1->ISR & 0x00000004 ));
      //lettura dato (azzeramento flag)
      ADC_val=ADC1->DR;
       //Conversione codice->tensione
      ADC_valc=(ADC_val*3300)/4096;
      printf("Il valore risultante dalla conversione e' %f mV\n",ADC_valc);
    }
   }
}
