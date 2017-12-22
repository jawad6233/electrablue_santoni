#
# arch/arm64/boot/Makefile
#
# This file is included by the global makefile so that you can add your own
# architecture-specific flags and dependencies.
#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 2012, ARM Ltd.
# Author: Will Deacon <will.deacon@arm.com>
#
# Based on the ia64 boot/Makefile.
#

include $(srctree)/arch/arm64/boot/dts/qcom/Makefile

targets := Image Image.gz

DTB_NAMES := $(subst $\",,$(CONFIG_BUILD_ARM64_APPENDED_DTB_IMAGE_NAMES))
ifneq ($(DTB_NAMES),)
DTB_LIST := $(addsuffix .dtb,$(DTB_NAMES))
DTB_OBJS := $(addprefix $(obj)/dts/,$(DTB_LIST))
else
<<<<<<< HEAD
DTB_LIST := $(dtb-y)
endif
DTB_OBJS := $(addprefix $(obj)/dts/qcom/,$(DTB_LIST))
=======
DTB_OBJS := $(shell find -L $(obj)/dts/ -name \*.dtb)
endif
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

$(obj)/Image: vmlinux FORCE
	$(call if_changed,objcopy)

<<<<<<< HEAD
=======
$(obj)/Image-dtb: $(obj)/Image $(DTB_OBJS) FORCE
	$(call if_changed,cat)

>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
$(obj)/Image.gz: $(obj)/Image FORCE
	$(call if_changed,gzip)

$(obj)/Image.gz-dtb: $(obj)/Image.gz $(DTB_OBJS) FORCE
	$(call if_changed,cat)

<<<<<<< HEAD
=======
$(obj)/Image-dtb-hdr: $(obj)/Image FORCE
	echo -n 'UNCOMPRESSED_IMG' > $@ && \
	$(call size_append, $(filter-out FORCE,$^)) >> $@

$(obj)/Image-dtb: $(obj)/Image-dtb-hdr $(obj)/Image $(DTB_OBJS) FORCE
	$(call if_changed,cat)

>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
install: $(obj)/Image
	$(CONFIG_SHELL) $(srctree)/$(src)/install.sh $(KERNELRELEASE) \
	$(obj)/Image System.map "$(INSTALL_PATH)"

zinstall: $(obj)/Image.gz
	$(CONFIG_SHELL) $(srctree)/$(src)/install.sh $(KERNELRELEASE) \
	$(obj)/Image.gz System.map "$(INSTALL_PATH)"
