dtb-$(CONFIG_ARCH_MSM8940) += msm8940-0061.dtb 
	#msm8940-pmi8937-cdp.dtb \
	msm8940-pmi8937-mtp.dtb \
	msm8940-pmi8950-cdp.dtb \
	msm8940-pmi8950-mtp.dtb \
	msm8940-pmi8937-rcm.dtb \
	msm8940-pmi8950-rcm.dtb \
	msm8940-pmi8950-qrd-sku6.dtb \
	msm8940-pmi8950-qrd-sku7.dtb \
	msm8940-pmi8950-ext-codec-cdp.dtb \
	msm8940-pmi8940-mtp.dtb \
	msm8940-pmi8940-cdp.dtb \
	msm8940-pmi8940-rcm.dtb \
	msm8940-pmi8940-qrd-sku7.dtb


ifeq ($(CONFIG_ARM64),y)
always          := $(dtb-y)
subdir-y        := $(dts-dirs)
else
targets += dtbs
targets += $(addprefix ../, $(dtb-y))

$(obj)/../%.dtb: $(src)/%.dts FORCE
        $(call if_changed_dep,dtc)

dtbs: $(addprefix $(obj)/,$(dtb-y))
endif
clean-files := *.dtb
