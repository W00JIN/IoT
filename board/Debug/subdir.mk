################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../._main.c \
../command.c \
../module-api.c \
../mqtt-api.c \
../pwm-api.c \
../request.c \
../sh1106.c \
../websocket-api.c \
../wifi-api.c \
../wifi-auto.c 

OBJS += \
./._main.o \
./command.o \
./module-api.o \
./mqtt-api.o \
./pwm-api.o \
./request.o \
./sh1106.o \
./websocket-api.o \
./wifi-api.o \
./wifi-auto.o 

C_DEPS += \
./._main.d \
./command.d \
./module-api.d \
./mqtt-api.d \
./pwm-api.d \
./request.d \
./sh1106.d \
./websocket-api.d \
./wifi-api.d \
./wifi-auto.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARTIK GCC C Compiler'
	arm-none-eabi-gcc  -D__TINYARA__ -I/Applications/ARTIK/SDK/A05x/v1.7.1/libsdk/A053/extra/include -I/Applications/ARTIK/SDK/A05x/v1.7.1/libsdk/A053/extra/include/apps -I/Applications/ARTIK/SDK/A05x/v1.7.1/libsdk/A053/extra/framework_include -I/Applications/ARTIK/SDK/A05x/v1.7.1/libsdk/A053/extra/apps_include -I/Applications/ARTIK/SDK/A05x/v1.7.1/libsdk/A053/extra/apps_include/netutils -I/Applications/ARTIK/SDK/A05x/v1.7.1/libsdk/A053/extra/artik-sdk_include -I/Applications/ARTIK/SDK/A05x/v1.7.1/libsdk/A053/extra/artik-sdk_include/base -I/Applications/ARTIK/SDK/A05x/v1.7.1/libsdk/A053/extra/artik-sdk_include/connectivity -I/Applications/ARTIK/SDK/A05x/v1.7.1/libsdk/A053/extra/artik-sdk_include/systemio -I/Applications/ARTIK/SDK/A05x/v1.7.1/libsdk/A053/extra/artik-sdk_include/wifi -I/Applications/ARTIK/SDK/A05x/v1.7.1/libsdk/A053/extra/artik-sdk_include/lwm2m -I/Applications/ARTIK/SDK/A05x/v1.7.1/libsdk/A053/extra/artik-sdk_include/mqtt -O0 -ffunction-sections -fdata-sections -fno-builtin -fno-strict-aliasing -fno-strength-reduce -fomit-frame-pointer -Wstrict-prototypes -Wshadow -Wno-implicit-function-declaration -Wno-unused-function -Wno-unused-but-set-variable -DAPP_STACKSIZE=18432 -DAPP_PRIORITY=100 -g -DCONFIG_ARTIK_IDE_INTERGRATE -mcpu=cortex-r4 -mfpu=vfpv3 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


