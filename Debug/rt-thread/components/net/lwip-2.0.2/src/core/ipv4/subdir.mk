################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/components/net/lwip-2.0.2/src/core/ipv4/autoip.c \
../rt-thread/components/net/lwip-2.0.2/src/core/ipv4/dhcp.c \
../rt-thread/components/net/lwip-2.0.2/src/core/ipv4/etharp.c \
../rt-thread/components/net/lwip-2.0.2/src/core/ipv4/icmp.c \
../rt-thread/components/net/lwip-2.0.2/src/core/ipv4/igmp.c \
../rt-thread/components/net/lwip-2.0.2/src/core/ipv4/ip4.c \
../rt-thread/components/net/lwip-2.0.2/src/core/ipv4/ip4_addr.c \
../rt-thread/components/net/lwip-2.0.2/src/core/ipv4/ip4_frag.c 

OBJS += \
./rt-thread/components/net/lwip-2.0.2/src/core/ipv4/autoip.o \
./rt-thread/components/net/lwip-2.0.2/src/core/ipv4/dhcp.o \
./rt-thread/components/net/lwip-2.0.2/src/core/ipv4/etharp.o \
./rt-thread/components/net/lwip-2.0.2/src/core/ipv4/icmp.o \
./rt-thread/components/net/lwip-2.0.2/src/core/ipv4/igmp.o \
./rt-thread/components/net/lwip-2.0.2/src/core/ipv4/ip4.o \
./rt-thread/components/net/lwip-2.0.2/src/core/ipv4/ip4_addr.o \
./rt-thread/components/net/lwip-2.0.2/src/core/ipv4/ip4_frag.o 

C_DEPS += \
./rt-thread/components/net/lwip-2.0.2/src/core/ipv4/autoip.d \
./rt-thread/components/net/lwip-2.0.2/src/core/ipv4/dhcp.d \
./rt-thread/components/net/lwip-2.0.2/src/core/ipv4/etharp.d \
./rt-thread/components/net/lwip-2.0.2/src/core/ipv4/icmp.d \
./rt-thread/components/net/lwip-2.0.2/src/core/ipv4/igmp.d \
./rt-thread/components/net/lwip-2.0.2/src/core/ipv4/ip4.d \
./rt-thread/components/net/lwip-2.0.2/src/core/ipv4/ip4_addr.d \
./rt-thread/components/net/lwip-2.0.2/src/core/ipv4/ip4_frag.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/components/net/lwip-2.0.2/src/core/ipv4/%.o: ../rt-thread/components/net/lwip-2.0.2/src/core/ipv4/%.c
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -ffunction-sections -fdata-sections -Wall  -g -gdwarf-2 -DDEBUG -I"D:\RT-ThreadStudio\workspace\1234\drivers" -I"D:\RT-ThreadStudio\workspace\1234\libraries" -I"D:\RT-ThreadStudio\workspace\1234\libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\RT-ThreadStudio\workspace\1234\libraries\CMSIS\Include" -I"D:\RT-ThreadStudio\workspace\1234\libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\RT-ThreadStudio\workspace\1234" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\dfs\filesystems\elmfat" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\dfs\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\drivers\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\drivers\spi\sfud\inc" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\drivers\spi" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\finsh" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\libc\compilers\minilibc" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\lwip-2.0.2\src\arch\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\lwip-2.0.2\src\include\netif" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\lwip-2.0.2\src\include\posix" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\lwip-2.0.2\src\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\lwip-2.0.2\src" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\netdev\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\libcpu\arm\common" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\libcpu\arm\cortex-m4" -include"D:\RT-ThreadStudio\workspace\1234\rtconfig_preinc.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

