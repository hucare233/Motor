#include "music.h"

uint16_t arry_m[22] = {0,
                       262, 294, 330, 349, 392, 440, 494,
                       523, 587, 659, 698, 784, 880, 988,
                       1046,1175,1318,1397,1568,1760,1976
                      };

void Time_Control_Beep_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    /* TIM3 clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);

    TIM_TimeBaseInitStruct.TIM_Prescaler     = 8399;
    TIM_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period        = 10000/262-1;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    //TIM_TimeBaseInitStruct.TIM_RepetitionCounter =
    TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);


    //TIM13对应引脚复用映射
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_TIM1); //GPIOA9复用为定时器14PWM


    //time13 pwn 端口配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ; //GPIOA9
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;	//速度50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
    GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化Pf9


    /* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    //TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM1, ENABLE);

    TIM_Cmd(TIM1,ENABLE);

}

void Set_Beep_Fre(uint32_t fre)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_Prescaler     = 8399;
    TIM_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_Period     = 10000/fre-1;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);

}

//
void Time1_Beep(void)
{
    int i = 10;
    int flag = 1;
    while(1)
    {
        if(flag)
        {
            i++;
            TIM_SetCompare1(TIM1,i);

            Delay_ms(50);
            if(i==38)
                flag = 0;

        }
        else
        {
            i--;
            TIM_SetCompare1(TIM1,i);

            Delay_ms(50);
            if(i==0)
                flag = 1;
        }

    }
}

void Play_Pre(uint8_t a,uint8_t b,uint8_t sleep)
{
    uint8_t d = (b-1)*7 +a;
    Set_Beep_Fre(arry_m[d]);
    TIM_SetCompare1(TIM1,2000/arry_m[d]);

    Delay_ms(sleep*200);

}

void Play_Music(unsigned char *music,int songlenth)
{
    unsigned int i = 0;
    for(; i<songlenth/3; i++)
    {
        Play_Pre(music[i*3],music[i*3+1],music[i*3+2]);

    }
    TIM_SetCompare1(TIM1,0);
}
