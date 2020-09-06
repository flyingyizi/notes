######################################
# source
######################################

# ASM sources
ASM_SOURCES_S =  \
        ${wildcard ./Middlewares/LiteOS/arch/arm/cortex-m/cortex-m4/gcc/*.S}

BSP_SRC = \
        ${wildcard ./Middlewares/LiteOS/bsp/common/*.c} 
BSP_SRC_EXCLUDE = \
        ./Middlewares/LiteOS/bsp/common/console.c \
        ./Middlewares/LiteOS/bsp/common/virtual_serial.c

# C sources
BSP_SRC := $(filter-out $(BSP_SRC_EXCLUDE), $(BSP_SRC))
        C_SOURCES += $(BSP_SRC)

# # C includes
BSP_INC = \
        -I ./Middlewares/LiteOS/bsp/common \
        -I ./Middlewares/LiteOS/bsp/include \
        -I ./Middlewares/LiteOS/bsp/hw/include 
        C_INCLUDES += $(BSP_INC)

# C sources
ARCH_SRC =  \
        ${wildcard ./Middlewares/LiteOS/arch/common/*.c} \
        ${wildcard ./Middlewares/LiteOS/arch/arm/cortex-m/src/*.c} \
        ${wildcard ./Middlewares/LiteOS/arch/arm/cortex-m/cortex-m4/*.c}
        C_SOURCES += $(ARCH_SRC)

# ${wildcard ./Middlewares/LiteOS/arch/arm/cortex-a/src/*.c} \

# C includes
ARCH_INC = \
        -I ./Middlewares/LiteOS/arch/common \
        -I ./Middlewares/LiteOS/arch/arm/include \
        -I ./Middlewares/LiteOS/arch/arm/cortex-m/include \
        -I ./Middlewares/LiteOS/arch/arm/cortex-m/src/include \
        -I ./Middlewares/LiteOS/arch/arm/cortex-m/cortex-m4/include
        C_INCLUDES += $(ARCH_INC)

# C includes
C_INCLUDES += -I./Middlewares/LiteOS/OS_CONFIG -I./Middlewares/LiteOS/OS_CONFIG/include       
