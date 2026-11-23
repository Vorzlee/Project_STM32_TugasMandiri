
#include "stm32f4xx.h"                 // bagian ini memanggil library register STM32F4

int main(void) {                       // bagian ini merupakan fungsi utama program

    // bagian ini mengaktifkan clock GPIOA dan TIM2
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // bagian ini mengaktifkan clock GPIOA
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // bagian ini mengaktifkan clock TIM2

    // bagian ini mengatur PA0 sebagai input pull-up
    GPIOA->MODER &= ~(3U << 0);         // bagian ini mengatur PA0 sebagai input
    GPIOA->PUPDR &= ~(3U << 0);         // bagian ini mereset konfigurasi pull-up/pull-down PA0
    GPIOA->PUPDR |= (1U << 0);          // bagian ini mengaktifkan pull-up pada PA0

    // bagian ini mengatur PA1 sebagai output LED
    GPIOA->MODER &= ~(3U << 2);         // bagian ini mereset mode PA1
    GPIOA->MODER |= (1U << 2);          // bagian ini mengatur PA1 sebagai output

    // bagian ini mengatur kondisi awal LED agar mati
    GPIOA->BSRR = (1U << 17);           // bagian ini memberikan logika LOW pada PA1

    // bagian ini mengatur konfigurasi TIM2 untuk interval 200 ms
    TIM2->CR1 = 0;                      // bagian ini memastikan TIM2 dalam kondisi berhenti
    TIM2->PSC = 15999;                  // bagian ini mengatur prescaler menjadi 1000 Hz
    TIM2->ARR = 199;                    // bagian ini mengatur periode timer menjadi 200 ms
    TIM2->CNT = 0;                      // bagian ini mengatur nilai awal counter menjadi 0

    TIM2->EGR = TIM_EGR_UG;             // bagian ini memperbarui nilai register timer
    TIM2->SR = 0;                       // bagian ini membersihkan flag timer

    while (1) {                         // bagian ini menjalankan program secara terus-menerus

        // bagian ini membaca kondisi pushbutton PA0
        if ((GPIOA->IDR & (1U << 0)) == 0) { // bagian ini mengecek apakah tombol sedang ditekan

            TIM2->CR1 |= TIM_CR1_CEN;   // bagian ini menjalankan TIM2 ketika tombol ditekan

            // bagian ini memeriksa apakah timer sudah mencapai 200 ms
            if (TIM2->SR & TIM_SR_UIF) { // bagian ini mengecek flag update timer

                TIM2->SR &= ~TIM_SR_UIF; // bagian ini membersihkan flag update timer
                GPIOA->ODR ^= (1U << 1); // bagian ini melakukan toggle pada LED PA1
            }

        } else {                        // bagian ini dijalankan ketika tombol dilepas

            TIM2->CR1 &= ~TIM_CR1_CEN;  // bagian ini menghentikan TIM2
            TIM2->CNT = 0;              // bagian ini mereset nilai counter timer
            TIM2->SR = 0;               // bagian ini membersihkan flag timer

            GPIOA->BSRR = (1U << 17);   // bagian ini mematikan LED ketika tombol dilepas
        }
    }
}
