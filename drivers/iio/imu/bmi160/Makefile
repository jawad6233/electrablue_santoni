#
#Makefile for BMI160 IIO Driver
#
<<<<<<< HEAD
obj-y   += bmi160_i2c.o bmi160_core.o bmi160.o 
obj-y   += bmi160_ring.o
=======
obj-${CONFIG_SENSORS_BMI160_IIO}   += bmi160_i2c.o bmi160_core.o bmi160.o
obj-${CONFIG_SENSORS_BMI160_IIO_RING}   += bmi160_ring.o
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

EXTRA_CFLAGS += -DBMI160_ENABLE_INT1

EXTRA_CFLAGS += -DBMI_USE_BASIC_I2C_FUNC
