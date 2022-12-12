#include <stdint.h>
#include "gpio.h"

#define PERIPH_BASE           0x40000000
#define APB1PERIPH_BASE       PERIPH_BASE
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x10000)
#define AHBPERIPH_BASE        (PERIPH_BASE + 0x20000)

#define GPIOC_BASE            (APB2PERIPH_BASE + 0x1000)
#define RCC_BASE              (AHBPERIPH_BASE + 0x00001000U)

#define CRL  0
#define CRH  4
#define IDR  8
#define ODR  12
#define BSRR 16
#define BRR  20

#define GPIO_INPUT_MODE           (0)
#define GPIO_OUTPUT_10MHz_MODE    (1)
#define GPIO_OUTPUT_2MHz_MODE     (2)
#define GPIO_OUTPUT_50MHz_MODE    (3)

/* Valid for input modes. */
#define GPIO_ANALOG_INPUT_CNF     (0 << 2)
#define GPIO_FLOATING_INPUT_CNF   (1 << 2)
#define GPIO_INPUT_PULLUP_CNF     (2 << 2)
#define GPIO_RESERVED_CNF         (3 << 2)

/* Valid for output modes. */
#define GPIO_OUTPUT_PUSHPULL_CNF  (0 << 2)
#define GPIO_OUTPUT_OPENDRAIN_CNF (1 << 2)
#define GPIO_ALT_PUSHPULL_CNF     (2 << 2)
#define GPIO_ALT_OPENDRAIN_CNF    (3 << 2)

#define RCC_APB2Periph_GPIOC             ((uint32_t)0x00000010)

#define AHBENR  20
#define APB2ENR 24
#define APB1ENR 28

#define writel(address, value)     (*(volatile unsigned long*)(address) = (value))
#define readl(address)             ((unsigned long)(*(volatile unsigned long*)(address)))


void gpio_init(void)
{
    uint32_t reg;

    /* enable clock for GPIOC */
    reg = readl(RCC_BASE+APB2ENR);
    reg |= RCC_APB2Periph_GPIOC;
    writel(RCC_BASE+APB2ENR, reg);

    /* init PC6 and PC7 as output */
    reg = readl(GPIOC_BASE+CRL);
    reg &= ~0xFF000000;
    reg |= ((GPIO_OUTPUT_10MHz_MODE | GPIO_OUTPUT_PUSHPULL_CNF)<<28) +
           ((GPIO_OUTPUT_10MHz_MODE | GPIO_OUTPUT_PUSHPULL_CNF)<<24);
    writel(GPIOC_BASE+CRL, reg);
}

void led1_on(void)
{
    uint32_t reg;

    reg = readl(GPIOC_BASE+IDR);
    reg |= (1<<6);
    writel(GPIOC_BASE+ODR, reg);
}

void led1_off(void)
{
    uint32_t reg;

    reg = readl(GPIOC_BASE+IDR);
    reg &= ~(1<<6);
    writel(GPIOC_BASE+ODR, reg);
}

void led1_toggle(void)
{
    uint32_t reg;

    reg = readl(GPIOC_BASE+IDR);
    if(reg&(1<<6))
    {
        reg &= ~(1<<6);
    }
    else
    {
        reg |= (1<<6);
    }
    writel(GPIOC_BASE+ODR, reg);
}

void led2_on(void)
{
    uint32_t reg;

    reg = readl(GPIOC_BASE+IDR);
    reg |= (1<<7);
    writel(GPIOC_BASE+ODR, reg);
}

void led2_off(void)
{
    uint32_t reg;

    reg = readl(GPIOC_BASE+IDR);
    reg &= ~(1<<7);
    writel(GPIOC_BASE+ODR, reg);
}

void led2_toggle(void)
{
    uint32_t reg;

    reg = readl(GPIOC_BASE+IDR);
    if(reg&(1<<7))
    {
        reg &= ~(1<<7);
    }
    else
    {
        reg |= (1<<7);
    }
    writel(GPIOC_BASE+ODR, reg);
}
