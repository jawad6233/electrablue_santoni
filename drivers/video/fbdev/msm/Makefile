<<<<<<< HEAD
ifeq ($(CONFIG_FB_MSM_MDSS_COMMON),y)
=======
ifneq ($(filter y, $(CONFIG_FB_MSM_MDSS_COMMON) $(CONFIG_MSM_DBA)),)
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
obj-y += ../../msm/
else
obj-y := msm_fb.o

# MDP DMA/PPP engine
#
obj-y += mdp.o mdp_scale_tables.o mdp_ppp.o

# MDDI interface
#
obj-y += mddi.o

# MDDI client/panel drivers
#
obj-y += mddi_client_dummy.o
obj-y += mddi_client_toshiba.o
obj-y += mddi_client_nt35399.o
endif
