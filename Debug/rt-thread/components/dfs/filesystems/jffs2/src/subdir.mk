################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/components/dfs/filesystems/jffs2/src/build.c \
../rt-thread/components/dfs/filesystems/jffs2/src/compr.c \
../rt-thread/components/dfs/filesystems/jffs2/src/compr_rtime.c \
../rt-thread/components/dfs/filesystems/jffs2/src/compr_rubin.c \
../rt-thread/components/dfs/filesystems/jffs2/src/compr_zlib.c \
../rt-thread/components/dfs/filesystems/jffs2/src/debug.c \
../rt-thread/components/dfs/filesystems/jffs2/src/dir-ecos.c \
../rt-thread/components/dfs/filesystems/jffs2/src/erase.c \
../rt-thread/components/dfs/filesystems/jffs2/src/flashio.c \
../rt-thread/components/dfs/filesystems/jffs2/src/fs-ecos.c \
../rt-thread/components/dfs/filesystems/jffs2/src/gc.c \
../rt-thread/components/dfs/filesystems/jffs2/src/gcthread.c \
../rt-thread/components/dfs/filesystems/jffs2/src/malloc-ecos.c \
../rt-thread/components/dfs/filesystems/jffs2/src/nodelist.c \
../rt-thread/components/dfs/filesystems/jffs2/src/nodemgmt.c \
../rt-thread/components/dfs/filesystems/jffs2/src/read.c \
../rt-thread/components/dfs/filesystems/jffs2/src/readinode.c \
../rt-thread/components/dfs/filesystems/jffs2/src/scan.c \
../rt-thread/components/dfs/filesystems/jffs2/src/write.c 

OBJS += \
./rt-thread/components/dfs/filesystems/jffs2/src/build.o \
./rt-thread/components/dfs/filesystems/jffs2/src/compr.o \
./rt-thread/components/dfs/filesystems/jffs2/src/compr_rtime.o \
./rt-thread/components/dfs/filesystems/jffs2/src/compr_rubin.o \
./rt-thread/components/dfs/filesystems/jffs2/src/compr_zlib.o \
./rt-thread/components/dfs/filesystems/jffs2/src/debug.o \
./rt-thread/components/dfs/filesystems/jffs2/src/dir-ecos.o \
./rt-thread/components/dfs/filesystems/jffs2/src/erase.o \
./rt-thread/components/dfs/filesystems/jffs2/src/flashio.o \
./rt-thread/components/dfs/filesystems/jffs2/src/fs-ecos.o \
./rt-thread/components/dfs/filesystems/jffs2/src/gc.o \
./rt-thread/components/dfs/filesystems/jffs2/src/gcthread.o \
./rt-thread/components/dfs/filesystems/jffs2/src/malloc-ecos.o \
./rt-thread/components/dfs/filesystems/jffs2/src/nodelist.o \
./rt-thread/components/dfs/filesystems/jffs2/src/nodemgmt.o \
./rt-thread/components/dfs/filesystems/jffs2/src/read.o \
./rt-thread/components/dfs/filesystems/jffs2/src/readinode.o \
./rt-thread/components/dfs/filesystems/jffs2/src/scan.o \
./rt-thread/components/dfs/filesystems/jffs2/src/write.o 

C_DEPS += \
./rt-thread/components/dfs/filesystems/jffs2/src/build.d \
./rt-thread/components/dfs/filesystems/jffs2/src/compr.d \
./rt-thread/components/dfs/filesystems/jffs2/src/compr_rtime.d \
./rt-thread/components/dfs/filesystems/jffs2/src/compr_rubin.d \
./rt-thread/components/dfs/filesystems/jffs2/src/compr_zlib.d \
./rt-thread/components/dfs/filesystems/jffs2/src/debug.d \
./rt-thread/components/dfs/filesystems/jffs2/src/dir-ecos.d \
./rt-thread/components/dfs/filesystems/jffs2/src/erase.d \
./rt-thread/components/dfs/filesystems/jffs2/src/flashio.d \
./rt-thread/components/dfs/filesystems/jffs2/src/fs-ecos.d \
./rt-thread/components/dfs/filesystems/jffs2/src/gc.d \
./rt-thread/components/dfs/filesystems/jffs2/src/gcthread.d \
./rt-thread/components/dfs/filesystems/jffs2/src/malloc-ecos.d \
./rt-thread/components/dfs/filesystems/jffs2/src/nodelist.d \
./rt-thread/components/dfs/filesystems/jffs2/src/nodemgmt.d \
./rt-thread/components/dfs/filesystems/jffs2/src/read.d \
./rt-thread/components/dfs/filesystems/jffs2/src/readinode.d \
./rt-thread/components/dfs/filesystems/jffs2/src/scan.d \
./rt-thread/components/dfs/filesystems/jffs2/src/write.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/components/dfs/filesystems/jffs2/src/%.o: ../rt-thread/components/dfs/filesystems/jffs2/src/%.c
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -ffunction-sections -fdata-sections -Wall  -g -gdwarf-2 -DDEBUG -I"D:\RT-ThreadStudio\workspace\1234\drivers" -I"D:\RT-ThreadStudio\workspace\1234\libraries" -I"D:\RT-ThreadStudio\workspace\1234\libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\RT-ThreadStudio\workspace\1234\libraries\CMSIS\Include" -I"D:\RT-ThreadStudio\workspace\1234\libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\RT-ThreadStudio\workspace\1234" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\dfs\filesystems\jffs2\cyg\compress" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\dfs\filesystems\jffs2\cyg" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\dfs\filesystems\jffs2\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\dfs\filesystems\jffs2\kernel" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\dfs\filesystems\jffs2\src" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\dfs\filesystems\jffs2" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\dfs\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\drivers\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\drivers\spi\sfud\inc" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\drivers\spi" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\finsh" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\libc\compilers\minilibc" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\lwip-2.0.2\src\arch\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\lwip-2.0.2\src\include\netif" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\lwip-2.0.2\src\include\posix" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\lwip-2.0.2\src\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\lwip-2.0.2\src" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\netdev\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\libcpu\arm\common" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\libcpu\arm\cortex-m4" -include"D:\RT-ThreadStudio\workspace\1234\rtconfig_preinc.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

