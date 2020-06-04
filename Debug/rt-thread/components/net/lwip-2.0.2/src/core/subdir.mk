################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/components/net/lwip-2.0.2/src/core/def.c \
../rt-thread/components/net/lwip-2.0.2/src/core/dns.c \
../rt-thread/components/net/lwip-2.0.2/src/core/inet_chksum.c \
../rt-thread/components/net/lwip-2.0.2/src/core/init.c \
../rt-thread/components/net/lwip-2.0.2/src/core/ip.c \
../rt-thread/components/net/lwip-2.0.2/src/core/memp.c \
../rt-thread/components/net/lwip-2.0.2/src/core/netif.c \
../rt-thread/components/net/lwip-2.0.2/src/core/pbuf.c \
../rt-thread/components/net/lwip-2.0.2/src/core/raw.c \
../rt-thread/components/net/lwip-2.0.2/src/core/stats.c \
../rt-thread/components/net/lwip-2.0.2/src/core/sys.c \
../rt-thread/components/net/lwip-2.0.2/src/core/tcp.c \
../rt-thread/components/net/lwip-2.0.2/src/core/tcp_in.c \
../rt-thread/components/net/lwip-2.0.2/src/core/tcp_out.c \
../rt-thread/components/net/lwip-2.0.2/src/core/timeouts.c \
../rt-thread/components/net/lwip-2.0.2/src/core/udp.c 

OBJS += \
./rt-thread/components/net/lwip-2.0.2/src/core/def.o \
./rt-thread/components/net/lwip-2.0.2/src/core/dns.o \
./rt-thread/components/net/lwip-2.0.2/src/core/inet_chksum.o \
./rt-thread/components/net/lwip-2.0.2/src/core/init.o \
./rt-thread/components/net/lwip-2.0.2/src/core/ip.o \
./rt-thread/components/net/lwip-2.0.2/src/core/memp.o \
./rt-thread/components/net/lwip-2.0.2/src/core/netif.o \
./rt-thread/components/net/lwip-2.0.2/src/core/pbuf.o \
./rt-thread/components/net/lwip-2.0.2/src/core/raw.o \
./rt-thread/components/net/lwip-2.0.2/src/core/stats.o \
./rt-thread/components/net/lwip-2.0.2/src/core/sys.o \
./rt-thread/components/net/lwip-2.0.2/src/core/tcp.o \
./rt-thread/components/net/lwip-2.0.2/src/core/tcp_in.o \
./rt-thread/components/net/lwip-2.0.2/src/core/tcp_out.o \
./rt-thread/components/net/lwip-2.0.2/src/core/timeouts.o \
./rt-thread/components/net/lwip-2.0.2/src/core/udp.o 

C_DEPS += \
./rt-thread/components/net/lwip-2.0.2/src/core/def.d \
./rt-thread/components/net/lwip-2.0.2/src/core/dns.d \
./rt-thread/components/net/lwip-2.0.2/src/core/inet_chksum.d \
./rt-thread/components/net/lwip-2.0.2/src/core/init.d \
./rt-thread/components/net/lwip-2.0.2/src/core/ip.d \
./rt-thread/components/net/lwip-2.0.2/src/core/memp.d \
./rt-thread/components/net/lwip-2.0.2/src/core/netif.d \
./rt-thread/components/net/lwip-2.0.2/src/core/pbuf.d \
./rt-thread/components/net/lwip-2.0.2/src/core/raw.d \
./rt-thread/components/net/lwip-2.0.2/src/core/stats.d \
./rt-thread/components/net/lwip-2.0.2/src/core/sys.d \
./rt-thread/components/net/lwip-2.0.2/src/core/tcp.d \
./rt-thread/components/net/lwip-2.0.2/src/core/tcp_in.d \
./rt-thread/components/net/lwip-2.0.2/src/core/tcp_out.d \
./rt-thread/components/net/lwip-2.0.2/src/core/timeouts.d \
./rt-thread/components/net/lwip-2.0.2/src/core/udp.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/components/net/lwip-2.0.2/src/core/%.o: ../rt-thread/components/net/lwip-2.0.2/src/core/%.c
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -ffunction-sections -fdata-sections -Wall  -g -gdwarf-2 -DDEBUG -I"D:\RT-ThreadStudio\workspace\1234\drivers" -I"D:\RT-ThreadStudio\workspace\1234\libraries" -I"D:\RT-ThreadStudio\workspace\1234\libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\RT-ThreadStudio\workspace\1234\libraries\CMSIS\Include" -I"D:\RT-ThreadStudio\workspace\1234\libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\RT-ThreadStudio\workspace\1234" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\dfs\filesystems\elmfat" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\dfs\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\drivers\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\drivers\spi\sfud\inc" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\drivers\spi" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\finsh" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\libc\compilers\minilibc" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\lwip-2.0.2\src\arch\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\lwip-2.0.2\src\include\netif" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\lwip-2.0.2\src\include\posix" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\lwip-2.0.2\src\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\lwip-2.0.2\src" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\netdev\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\libcpu\arm\common" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\libcpu\arm\cortex-m4" -include"D:\RT-ThreadStudio\workspace\1234\rtconfig_preinc.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

