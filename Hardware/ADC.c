#include "stm32f10x.h"
#include "bsp_gpio.h"

uint16_t AD_Value[4]; //全局变量保存ADC转换结果，4个通道就4个数据单元

void AD_Init(void) //与ADC_Init要区分一下
{
    //1.开启ADC时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6); //PCLK2就是APB2时钟（72MHz），72MHz/6=12MHz (ADC时钟不能超过14MHz)
    //2.配置GPIO
    BSP_GPIO_InitPin(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3, GPIO_Mode_AIN, GPIO_Speed_50MHz); //注意要配置为模拟输入AIN模式
    //3.配置多路开关，需要加通道就继续加配置函数就行
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5); //ADC1的第0通道，序列选择1（序列1上面写入CH0通道），采样时间为55.5周期（采样周期越短，转换越快，越长，转换越稳定）
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_55Cycles5);
    //4.配置ADC
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;//扫描模式
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //连续转换模式 <-
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//触发源：使用软件触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//数据对齐：右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 4; //扫描模式下，选用多少个通道
    ADC_Init(ADC1, &ADC_InitStructure);
    //4.5 配置DMA
      //1.时钟使能
      RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
      //2.用结构体进行参数配置
      DMA_InitTypeDef DMA_InitStructure;
      DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR; //ADC_DR寄存器地址
      DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 外设：数据宽度:低16字节
      DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设：地址是否自增
      DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)AD_Value; // 存储器：起始地址
      DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; // 存储器：数据宽度
      DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // 存储器：地址是否自增
      DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; // 方向：外设到存储器
      DMA_InitStructure.DMA_BufferSize = 4; // 缓存区大小：数据单元个数
      DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //是否使用自动重装：使用，为循环模式 <-
      DMA_InitStructure.DMA_Priority = DMA_Priority_High; //高优先级(当多个通道同时请求时，优先级高的先被服务)
      DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // 选择软件触发or硬件触发：硬件触发，触发源是ADC1
  
      DMA_Init(DMA1_Channel1, &DMA_InitStructure);//通道不能任意选择
  
      //3.开关控制
      DMA_Cmd(DMA1_Channel1, ENABLE);
      //4.开启ADC的DMA请求
      ADC_DMACmd(ADC1, ENABLE);
    //5.开启ADC
    ADC_Cmd(ADC1, ENABLE);

    //6.校准ADC
    ADC_ResetCalibration(ADC1); //复位校准寄存器
    while(ADC_GetResetCalibrationStatus(ADC1)); //等待复位校准完成
    ADC_StartCalibration(ADC1); //开始校准
    while(ADC_GetCalibrationStatus(ADC1)); //等待校准完成

    //7.ADC触发
    ADC_SoftwareStartConvCmd(ADC1, ENABLE); //软件触发，ADC开始转换，DMA自动转运
}
