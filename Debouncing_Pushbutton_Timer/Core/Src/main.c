
#include "stm32f4xx.h"

volatile unsigned int pressed = 0;
volatile unsigned int button_state = 1;

void GPIO_EXTI_Init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // bagian ini menyalakan peripheral GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // bagian ini menyalakan peripheral SYSCFG

    GPIOA->MODER &= ~(3UL << (1 * 2)); //mengonfigurasi pin PA1 menjadi Input (bit 2 dan 3, (00) jadi input)
    GPIOA->MODER |= (1UL << (1 * 2)); //mengonfigurasi pin PA1 menjadi Output (bit 2 dan 3, (01) jadi Output)

    GPIOA->OTYPER &= ~(1UL << 1); // bagian ini mengatur tipe output PA1 menjadi Push-Pull (0)
    GPIOA->OSPEEDR &= ~(3UL << (1 * 2)); // bagian ini mengatur kecepatan output PA1 menjadi Low
    GPIOA->PUPDR &= ~(3UL << (1 * 2)); // bagian ini menonaktifkan pull-up dan pull-down PA1

    GPIOA->ODR &= ~(1UL << 1); // bagian ini membuat LED dalam kondisi mati

    GPIOA->MODER &= ~(3UL << (0 * 2)); // bagian ini mengonfigurasi pin PA0 menjadi Input

    // bagian ini mengaktifkan internal pull-up pada PA0
    GPIOA->PUPDR &= ~(3UL << (0 * 2));
    GPIOA->PUPDR |= (1UL << (0 * 2));

    // bagian ini membaca kondisi awal pushbutton
    button_state = (GPIOA->IDR & (1UL << 0)) ? 1 : 0;

    // bagian ini menghubungkan EXTI0 dengan pin PA0
    SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;

    // bagian ini mengonfigurasi jalur interrupt EXTI0
    EXTI->IMR |= (1UL << 0);
    EXTI->EMR &= ~(1UL << 0);

    // bagian ini mengaktifkan Falling Edge dan Rising Edge
    EXTI->FTSR |= (1UL << 0);
    EXTI->RTSR |= (1UL << 0);

    EXTI->PR = (1UL << 0); // bagian ini membersihkan pending interrupt

    // bagian ini mengaktifkan interrupt EXTI0 pada NVIC
    NVIC_SetPriority(EXTI0_IRQn, 1);
    NVIC_EnableIRQ(EXTI0_IRQn);
}

void TIM2_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // bagian ini menyalakan clock TIM2

    // bagian ini mengatur TIM2 untuk menghasilkan waktu 20 ms
    TIM2->PSC = 15999; // prescaler untuk menghasilkan frekuensi 1 kHz
    TIM2->ARR = 19; // nilai auto-reload untuk menghasilkan waktu 20 ms
    TIM2->CNT = 0; // bagian ini mereset counter TIM2

    TIM2->CR1 = TIM_CR1_OPM; // bagian ini mengaktifkan One Pulse Mode

    TIM2->DIER |= TIM_DIER_UIE; // bagian ini mengaktifkan interrupt update TIM2

    TIM2->EGR = TIM_EGR_UG; // bagian ini menghasilkan update event
    TIM2->SR = 0; // bagian ini membersihkan status interrupt

    // bagian ini mengaktifkan interrupt TIM2 pada NVIC
    NVIC_SetPriority(TIM2_IRQn, 2);
    NVIC_EnableIRQ(TIM2_IRQn);
}

void EXTI0_IRQHandler(void)
{
    if (EXTI->PR & (1UL << 0))
    {
        EXTI->IMR &= ~(1UL << 0); // bagian ini menonaktifkan EXTI0 selama debounce

        EXTI->PR = (1UL << 0); // bagian ini membersihkan pending interrupt

        // bagian ini mereset dan memulai TIM2
        TIM2->CR1 &= ~TIM_CR1_CEN;
        TIM2->CNT = 0;
        TIM2->SR = 0;
        TIM2->CR1 |= TIM_CR1_CEN;
    }
}

void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR &= ~TIM_SR_UIF; // bagian ini membersihkan update interrupt flag

        // bagian ini membaca kondisi pushbutton setelah 20 ms
        unsigned int current_state = (GPIOA->IDR & (1UL << 0)) ? 1 : 0;

        // bagian ini memeriksa apakah kondisi pushbutton berubah
        if (current_state != button_state)
        {
            button_state = current_state;

            // bagian ini mengaktifkan semaphore jika tombol benar-benar ditekan
            if (current_state == 0)
            {
                pressed = 1;
            }
        }

        EXTI->PR = (1UL << 0); // bagian ini membersihkan pending flag EXTI0

        EXTI->IMR |= (1UL << 0); // bagian ini mengaktifkan kembali EXTI0
    }
}

int main(void)
{
    TIM2_Init(); // bagian ini memanggil inisialisasi TIM2
    GPIO_EXTI_Init(); // bagian ini memanggil inisialisasi GPIO dan EXTI

    while (1)
    {
        unsigned int toggle_led = 0;

        // bagian ini memeriksa semaphore pressed
        __disable_irq();

        if (pressed)
        {
            pressed = 0; // bagian ini mereset semaphore
            toggle_led = 1;
        }

        __enable_irq();

        // bagian ini mengubah kondisi LED jika tombol ditekan
        if (toggle_led)
        {
            GPIOA->ODR ^= (1UL << 1);
        }
    }
}
