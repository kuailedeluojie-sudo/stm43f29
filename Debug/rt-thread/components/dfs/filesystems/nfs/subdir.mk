################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/components/dfs/filesystems/nfs/dfs_nfs.c \
../rt-thread/components/dfs/filesystems/nfs/mount_clnt.c \
../rt-thread/components/dfs/filesystems/nfs/mount_xdr.c \
../rt-thread/components/dfs/filesystems/nfs/nfs_auth.c \
../rt-thread/components/dfs/filesystems/nfs/nfs_clnt.c \
../rt-thread/components/dfs/filesystems/nfs/nfs_xdr.c 

OBJS += \
./rt-thread/components/dfs/filesystems/nfs/dfs_nfs.o \
./rt-thread/components/dfs/filesystems/nfs/mount_clnt.o \
./rt-thread/components/dfs/filesystems/nfs/mount_xdr.o \
./rt-thread/components/dfs/filesystems/nfs/nfs_auth.o \
./rt-thread/components/dfs/filesystems/nfs/nfs_clnt.o \
./rt-thread/components/dfs/filesystems/nfs/nfs_xdr.o 

C_DEPS += \
./rt-thread/components/dfs/filesystems/nfs/dfs_nfs.d \
./rt-thread/components/dfs/filesystems/nfs/mount_clnt.d \
./rt-thread/components/dfs/filesystems/nfs/mount_xdr.d \
./rt-thread/components/dfs/filesystems/nfs/nfs_auth.d \
./rt-thread/components/dfs/filesystems/nfs/nfs_clnt.d \
./rt-thread/components/dfs/filesystems/nfs/nfs_xdr.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/components/dfs/filesystems/nfs/%.o: ../rt-thread/components/dfs/filesystems/nfs/%.c
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -ffunction-sections -fdata-sections -Wall  -g -gdwarf-2 -DDEBUG -I"D:\RT-ThreadStudio\workspace\1234\drivers" -I"D:\RT-ThreadStudio\workspace\1234\libraries" -I"D:\RT-ThreadStudio\workspace\1234\libraries\CMSIS\Device\ST\STM32F4xx\Include" -I"D:\RT-ThreadStudio\workspace\1234\libraries\CMSIS\Include" -I"D:\RT-ThreadStudio\workspace\1234\libraries\STM32F4xx_HAL_Driver\Inc" -I"D:\RT-ThreadStudio\workspace\1234" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\dfs\filesystems\devfs" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\dfs\filesystems\nfs" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\dfs\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\drivers\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\drivers\spi\sfud\inc" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\drivers\spi" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\finsh" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\libc\compilers\minilibc" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\lwip-2.0.2\src\arch\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\lwip-2.0.2\src\include\netif" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\lwip-2.0.2\src\include\posix" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\lwip-2.0.2\src\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\lwip-2.0.2\src" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\components\net\netdev\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\include" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\libcpu\arm\common" -I"D:\RT-ThreadStudio\workspace\1234\rt-thread\libcpu\arm\cortex-m4" -include"D:\RT-ThreadStudio\workspace\1234\rtconfig_preinc.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

