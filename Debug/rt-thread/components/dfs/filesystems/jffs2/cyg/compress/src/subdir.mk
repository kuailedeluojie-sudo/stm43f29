################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/adler32.c \
../rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/compress.c \
../rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/deflate.c \
../rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/infback.c \
../rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/inffast.c \
../rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/inflate.c \
../rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/inftrees.c \
../rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/trees.c \
../rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/uncompr.c \
../rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/zutil.c 

OBJS += \
./rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/adler32.o \
./rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/compress.o \
./rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/deflate.o \
./rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/infback.o \
./rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/inffast.o \
./rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/inflate.o \
./rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/inftrees.o \
./rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/trees.o \
./rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/uncompr.o \
./rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/zutil.o 

C_DEPS += \
./rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/adler32.d \
./rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/compress.d \
./rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/deflate.d \
./rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/infback.d \
./rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/inffast.d \
./rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/inflate.d \
./rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/inftrees.d \
./rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/trees.d \
./rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/uncompr.d \
./rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/zutil.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/%.o: ../rt-thread/components/dfs/filesystems/jffs2/cyg/compress/src/%.c
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -ffunction-sections -fdata-sections -Wall  -g -gdwarf-2 -DDEBUG -I"D:\RT-ThreadStudio\workspace\1234\drivers" -I"D:\RT-ThreadStudio\workspace\1234\libraries" -I"D:\RT-ThreadStudio\workspace\1234\libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\RT-ThreadStudio\workspace\1234\libraries\CMSIS\Include" -I"D:\RT-ThreadStudio\workspace\1234\libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\RT-ThreadStudio\workspace\1234" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\dfs\filesystems\jffs2\cyg\compress" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\dfs\filesystems\jffs2\cyg" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\dfs\filesystems\jffs2\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\dfs\filesystems\jffs2\kernel" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\dfs\filesystems\jffs2\src" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\dfs\filesystems\jffs2" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\dfs\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\drivers\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\drivers\spi\sfud\inc" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\drivers\spi" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\finsh" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\libc\compilers\minilibc" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\lwip-2.0.2\src\arch\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\lwip-2.0.2\src\include\netif" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\lwip-2.0.2\src\include\posix" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\lwip-2.0.2\src\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\lwip-2.0.2\src" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\netdev\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\libcpu\arm\common" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\libcpu\arm\cortex-m4" -include"D:\RT-ThreadStudio\workspace\1234\rtconfig_preinc.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

