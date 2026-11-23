
#include "stm32f4xx.h"

int main(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // bagian ini mengaktifkan clock GPIOA

    GPIOA->MODER &= ~((3U << 0) | (3U << 2) | (3U << 4)); // bagian ini mengonfigurasi PA0,PA1,PA2 sebagai GPIO output
    GPIOA->MODER |=  ((1U << 0) | (1U << 2) | (1U << 4));

    // bagian ini menentukan tipe output masing2 PIN ke Push-Pull
    GPIOA->OTYPER &= ~((1U << 0) | (1U << 1) | (1U << 2));

    // No Pull-up, No Pull-down
    GPIOA->PUPDR &= ~((3U << 0) | (3U << 2) | (3U << 4));

    // Matikan semua LED pada kondisi awal
    GPIOA->BSRR = (1U << 16) | (1U << 17) | (1U << 18);

    // 3. Aktifkan clock TIM2, TIM3, dan TIM4
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN
                  | RCC_APB1ENR_TIM3EN
                  | RCC_APB1ENR_TIM4EN;

    // 4. Konfigurasi TIM2 untuk LED Hijau (PA2)
    TIM2->PSC = 1599;
    TIM2->ARR = 4999;
    TIM2->CNT = 0;
    TIM2->EGR = TIM_EGR_UG;
    TIM2->SR = 0;

    // 5. Konfigurasi TIM3 untuk LED Kuning (PA1)
    TIM3->PSC = 1599;
    TIM3->ARR = 2499;
    TIM3->CNT = 0;
    TIM3->EGR = TIM_EGR_UG;
    TIM3->SR = 0;

    // 6. Konfigurasi TIM4 untuk LED Merah (PA0)
    TIM4->PSC = 1599;
    TIM4->ARR = 1249;
    TIM4->CNT = 0;
    TIM4->EGR = TIM_EGR_UG;
    TIM4->SR = 0;

    // 7. Aktifkan ketiga timer
    TIM2->CR1 |= TIM_CR1_CEN;
    TIM3->CR1 |= TIM_CR1_CEN;
    TIM4->CR1 |= TIM_CR1_CEN;

    // 8. Perulangan utama
    while (1)
    {
        // TIM2: LED Hijau PA2
        if (TIM2->SR & TIM_SR_UIF)
        {
            TIM2->SR &= ~TIM_SR_UIF;
            GPIOA->ODR ^= (1U << 2);
        }

        // TIM3: LED Kuning PA1
        if (TIM3->SR & TIM_SR_UIF)
        {
            TIM3->SR &= ~TIM_SR_UIF;
            GPIOA->ODR ^= (1U << 1);
        }

        // TIM4: LED Merah PA0
        if (TIM4->SR & TIM_SR_UIF)
        {
            TIM4->SR &= ~TIM_SR_UIF;
            GPIOA->ODR ^= (1U << 0);
        }
    }
}
