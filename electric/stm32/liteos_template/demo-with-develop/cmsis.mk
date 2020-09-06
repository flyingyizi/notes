######################################
# source
######################################
CMSIS_SRC =  \
        ${wildcard ./Middlewares/LiteOS/osdepends/liteos/cmsis/*.c}
        C_SOURCES += $(CMSIS_SRC)

LITEOS_CMSIS = \
        -I ./Middlewares/LiteOS/osdepends/liteos/cmsis
        C_INCLUDES += $(LITEOS_CMSIS)
