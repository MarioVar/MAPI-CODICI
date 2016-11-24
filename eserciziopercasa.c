#include<stm32f30x.h>
#define FCK 72000000
#define TIMINT 1
#define UIF 1;
//variabile che mi serve per i led
unsigned int i=0;
//variabile che conta quante volte è stato premuto il tasto (user)
int count=0;
//variabile che mi serve per far si che i led si accendino solo se è stato premuto il tasto
int acceso=0;

int main(){
  //attivo le periferiche per GPIOA/E
  RCC->AHBENR |= 1<<21;
  RCC->AHBENR |= 1<<17;
  // attivo le periferiche tim2 e interrupt
  RCC->APB1ENR |= (uint32_t) 0x1;
  RCC->APB2ENR |= (uint32_t) 0x1;
  //inutile: azzera il moder di porta A
  GPIOA->MODER&=(~(unsigned int)3);
  //abilito tutti i led
  GPIOE->MODER|=0x55550000;
 
  TIM2->DIER|=(uint32_t)0x1;//abilito interruzioni timer
  //setto il prescaler a 1
  TIM2->PSC=0;
  //abilito il conteggio
  TIM2->CNT=0;
  //setto il registro arr
  TIM2->ARR=(uint32_t)(FCK*TIMINT-1)/4;//250ms
  TIM2->CR1|=0x1;//cen settato
  //SYSCFG->EXTICR[0]=(uint32_t)0x1;
  //istruzioni per l'abilitazioni delle interruzioni del tasto e del timer
  EXTI->IMR |= (uint32_t)0x1;
  EXTI->RTSR|=(uint32_t) 0x1;
  NVIC->ISER[0]|=1<<6;
  NVIC->ISER[0]|=1<<28;//ABILITO INTERRUZIONE TIMER2
  //ciclo infinito
  while(1);
}
void TIM2_IRQHandler(){//routine associata a tim2
  //azzero il bit di status così che ci potranno essere altre interruzioni timer
    TIM2->SR&=~UIF;
    //se il tasto è stato premuto almeno una volta allora si accenderanno i led
    if(acceso)
    {
      if(i==0)//per accendere il primo led
        i++;
      if(count%2==1){//se il tasto è premuto la 1° volta o la 3° ... ovvero un numero dispari di volte
        GPIOE->ODR=i<<8;
        //senso orario
        if(i<255)
          i=i*2;
        else//quando ho completato un giro reimposto i a 1
          i=1;
      }
      else if(count%2==0){
        GPIOE->ODR=i<<8;
        //senso antiorario
        if(i>0)
          i=i/2;
        if(i==0)//quando ho completato il giro inverso reimposto i a 256 (ultimo led)
          i=256;
       }
    }
}
void EXTI0_IRQHandler(){//routine interruzione tasto
  EXTI->PR&=0x1;//mettendo 1 è come se mettessi a zero il pe
  count++;//incremento il count relativo al tasto
  acceso=1;//ho premuto il tasto almeno una volta 
  
}