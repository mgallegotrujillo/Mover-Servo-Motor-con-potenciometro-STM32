/* Conexiones:
	Puerto A: motor		Puerto F
		Pin 0: Datos 			Pin 3: ADC3 canal 9
*/
// ****************************** LIBRERIAS *******************
		#include <stdio.h>
		#include "STM32F7xx.h"
// ************************************************************

// ****************************** VARIABLES GLOBALES **********
		short valor_adc=0;
		short grados=0; 
		short tiempo=100;
// ************************************************************

// ****************************** FUNCIONES *******************
		void conversion_adc(){
			ADC3->CR2 |=0x40000000;  // Inicio la conversion ADC
			while((ADC3->SR & 0x02)==1){} // Tiempo de espera mientras la conversion se realiza correctamente
		}
		void mover_servo(){
			tiempo=(valor_adc*2500)/4095; // Adapto el valor ADC para que quede dentro del rango del servo 
			GPIOA->ODR =0x01; // Pongo en alto la señal de control del servo 
			for(int i=10; i<tiempo; i++){} // Tiempo de espera que me genera un PWM variable 
			GPIOA->ODR =0x00; 
		}
// ************************************************************

// ****************************** INTERRUPCIONES **************
		extern "C"{
			void SysTick_Handler(void){
				mover_servo();
			}
			
			void ADC_IRQHandler(void){
					valor_adc=ADC3->DR; // Adquiero el valor ADC
			}
		}
// ************************************************************

// ****************************** MAIN ************************
int main(void){
	// **************************** PUERTOS *********************
			RCC->AHB1ENR |=0x23; // Habilito el puerto A y F
	// **********************************************************
	
	// **************************** PINES ***********************
			GPIOA->MODER |=0x01; // OUTPUT pin 0
			GPIOA->OTYPER |=0x00; // PUSH-PULL 
			GPIOA->OSPEEDR |=0x01; // MEDIUM-SPEED pin 0
			GPIOA->PUPDR |=0x01; // PULL-UP pin 0
		
			GPIOB->MODER |=15555; // OUTPUT pines 0 a 9
			GPIOB->OTYPER |=0x00; // PUSH-PULL
			GPIOB->OSPEEDR |=15555; // MEDIUM-SPEED 
			GPIOB->PUPDR |=15555; // PULL-UP 
	
			GPIOF->MODER |=0xC0; // ANALOGICO pin 3
	// **********************************************************
	
	// **************************** SYSTICK *********************
			SystemCoreClockUpdate();
			SysTick_Config(SystemCoreClock/100); // La interrupcion se generara cada 1ms 
	// **********************************************************
	
	// **************************** ADC *************************
			RCC->APB2ENR |=0x400; // Habilito el reloj del ADC3
			ADC3->CR1 |=0x20; // Activo la interrupcion EOCIE
			ADC3->CR2 |=0x400; // La conversion se realiza de forma secuencial no por saturacion (agrupacion)
			ADC3->CR2 |=0x01; // Activo el conversor ADC
			ADC3->SQR3 =0x09; // La conversion se hara por el canal 9 del ADC3
			NVIC_EnableIRQ(ADC_IRQn); // Activo el vector de interrupciones del ADC 
	// **********************************************************
	
	// **************************** BUCLE ***********************
			while(true){
				conversion_adc();// Realizamos la conversion ADC
				grados=(valor_adc*360)/4095; // Obtenemos los grados que se esta girando el potenciometro 
			}
	// **********************************************************
}
// ************************************************************