ccflags-y += -Idrivers/media/platform/msm/camera_v2
<<<<<<< HEAD
ccflags-y += -Idrivers/media/platform/msm/camera_v2/common
ccflags-y += -Idrivers/media/platform/msm/camera_v2/sensor/io
obj-$(CONFIG_MSM_CSID) += msm_ispif.o
=======
ccflags-y += -Idrivers/media/platform/msm/camera_v2/sensor/io
ccflags-y += -Idrivers/media/platform/msm/camera_v2/common/
ifeq ($(CONFIG_MSM_ISP_V1),y)
obj-$(CONFIG_MSM_CSID) += msm_ispif_32.o
else
obj-$(CONFIG_MSM_CSID) += msm_ispif.o
endif
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
