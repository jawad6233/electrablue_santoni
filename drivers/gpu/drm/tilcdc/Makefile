ccflags-y := -Iinclude/drm
ifeq (, $(findstring -W,$(EXTRA_CFLAGS)))
	ccflags-y += -Werror
endif

<<<<<<< HEAD
tilcdc-y := \
	tilcdc_crtc.o \
	tilcdc_tfp410.o \
	tilcdc_slave.o \
	tilcdc_panel.o \
=======
obj-$(CONFIG_DRM_TILCDC_SLAVE_COMPAT) += tilcdc_slave_compat.o \
					 tilcdc_slave_compat.dtb.o

tilcdc-y := \
	tilcdc_crtc.o \
	tilcdc_tfp410.o \
	tilcdc_panel.o \
	tilcdc_external.o \
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	tilcdc_drv.o

obj-$(CONFIG_DRM_TILCDC)	+= tilcdc.o
