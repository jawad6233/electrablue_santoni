/*
 * Copyright (C) 2013 Red Hat
 * Author: Rob Clark <robdclark@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __MDP5_KMS_H__
#define __MDP5_KMS_H__

#include "msm_drv.h"
#include "msm_kms.h"
#include "mdp/mdp_kms.h"
<<<<<<< HEAD
/* dynamic offsets used by mdp5.xml.h (initialized in mdp5_kms.c) */
#define MDP5_MAX_BASES		8
struct mdp5_sub_block {
	int	count;
	uint32_t base[MDP5_MAX_BASES];
};
struct mdp5_config {
	char  *name;
	struct mdp5_sub_block ctl;
	struct mdp5_sub_block pipe_vig;
	struct mdp5_sub_block pipe_rgb;
	struct mdp5_sub_block pipe_dma;
	struct mdp5_sub_block lm;
	struct mdp5_sub_block dspp;
	struct mdp5_sub_block ad;
	struct mdp5_sub_block intf;
};
extern const struct mdp5_config *mdp5_cfg;
#include "mdp5.xml.h"
=======
#include "mdp5_cfg.h"	/* must be included before mdp5.xml.h */
#include "mdp5.xml.h"
#include "mdp5_ctl.h"
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
#include "mdp5_smp.h"

struct mdp5_kms {
	struct mdp_kms base;

	struct drm_device *dev;

<<<<<<< HEAD
	int rev;
	const struct mdp5_config *hw_cfg;
=======
	struct mdp5_cfg_handler *cfg;
	uint32_t caps;	/* MDP capabilities (MDP_CAP_XXX bits) */

>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	/* mapper-id used to request GEM buffer mapped for scanout: */
	int id;
	struct msm_mmu *mmu;

<<<<<<< HEAD
	/* for tracking smp allocation amongst pipes: */
	mdp5_smp_state_t smp_state;
	struct mdp5_client_smp_state smp_client_state[CID_MAX];
	int smp_blk_cnt;
=======
	struct mdp5_smp *smp;
	struct mdp5_ctl_manager *ctlm;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

	/* io/register spaces: */
	void __iomem *mmio, *vbif;

	struct regulator *vdd;

	struct clk *axi_clk;
	struct clk *ahb_clk;
	struct clk *src_clk;
	struct clk *core_clk;
	struct clk *lut_clk;
	struct clk *vsync_clk;

<<<<<<< HEAD
	struct hdmi *hdmi;

	struct mdp_irq error_handler;
};
#define to_mdp5_kms(x) container_of(x, struct mdp5_kms, base)

/* platform config data (ie. from DT, or pdata) */
struct mdp5_platform_config {
	struct iommu_domain *iommu;
	uint32_t max_clk;
	int smp_blk_cnt;
=======
	/*
	 * lock to protect access to global resources: ie., following register:
	 *	- REG_MDP5_MDP_DISP_INTF_SEL
	 */
	spinlock_t resource_lock;

	struct mdp_irq error_handler;

	struct {
		volatile unsigned long enabled_mask;
		struct irq_domain *domain;
	} irqcontroller;
};
#define to_mdp5_kms(x) container_of(x, struct mdp5_kms, base)

struct mdp5_plane_state {
	struct drm_plane_state base;

	/* aligned with property */
	uint8_t premultiplied;
	uint8_t zpos;
	uint8_t alpha;

	/* assigned by crtc blender */
	enum mdp_mixer_stage_id stage;

	/* some additional transactional status to help us know in the
	 * apply path whether we need to update SMP allocation, and
	 * whether current update is still pending:
	 */
	bool mode_changed : 1;
	bool pending : 1;
};
#define to_mdp5_plane_state(x) \
		container_of(x, struct mdp5_plane_state, base)

enum mdp5_intf_mode {
	MDP5_INTF_MODE_NONE = 0,

	/* Modes used for DSI interface (INTF_DSI type): */
	MDP5_INTF_DSI_MODE_VIDEO,
	MDP5_INTF_DSI_MODE_COMMAND,

	/* Modes used for WB interface (INTF_WB type):  */
	MDP5_INTF_WB_MODE_BLOCK,
	MDP5_INTF_WB_MODE_LINE,
};

struct mdp5_interface {
	int num; /* display interface number */
	enum mdp5_intf_type type;
	enum mdp5_intf_mode mode;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
};

static inline void mdp5_write(struct mdp5_kms *mdp5_kms, u32 reg, u32 data)
{
	msm_writel(data, mdp5_kms->mmio + reg);
}

static inline u32 mdp5_read(struct mdp5_kms *mdp5_kms, u32 reg)
{
	return msm_readl(mdp5_kms->mmio + reg);
}

static inline const char *pipe2name(enum mdp5_pipe pipe)
{
	static const char *names[] = {
#define NAME(n) [SSPP_ ## n] = #n
		NAME(VIG0), NAME(VIG1), NAME(VIG2),
		NAME(RGB0), NAME(RGB1), NAME(RGB2),
		NAME(DMA0), NAME(DMA1),
		NAME(VIG3), NAME(RGB3),
#undef NAME
	};
	return names[pipe];
}

<<<<<<< HEAD
static inline uint32_t pipe2flush(enum mdp5_pipe pipe)
{
	switch (pipe) {
	case SSPP_VIG0: return MDP5_CTL_FLUSH_VIG0;
	case SSPP_VIG1: return MDP5_CTL_FLUSH_VIG1;
	case SSPP_VIG2: return MDP5_CTL_FLUSH_VIG2;
	case SSPP_RGB0: return MDP5_CTL_FLUSH_RGB0;
	case SSPP_RGB1: return MDP5_CTL_FLUSH_RGB1;
	case SSPP_RGB2: return MDP5_CTL_FLUSH_RGB2;
	case SSPP_DMA0: return MDP5_CTL_FLUSH_DMA0;
	case SSPP_DMA1: return MDP5_CTL_FLUSH_DMA1;
	case SSPP_VIG3: return MDP5_CTL_FLUSH_VIG3;
	case SSPP_RGB3: return MDP5_CTL_FLUSH_RGB3;
	default:        return 0;
	}
}

=======
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
static inline int pipe2nclients(enum mdp5_pipe pipe)
{
	switch (pipe) {
	case SSPP_RGB0:
	case SSPP_RGB1:
	case SSPP_RGB2:
	case SSPP_RGB3:
		return 1;
	default:
		return 3;
	}
}

<<<<<<< HEAD
static inline enum mdp5_client_id pipe2client(enum mdp5_pipe pipe, int plane)
{
	WARN_ON(plane >= pipe2nclients(pipe));
	switch (pipe) {
	case SSPP_VIG0: return CID_VIG0_Y + plane;
	case SSPP_VIG1: return CID_VIG1_Y + plane;
	case SSPP_VIG2: return CID_VIG2_Y + plane;
	case SSPP_RGB0: return CID_RGB0;
	case SSPP_RGB1: return CID_RGB1;
	case SSPP_RGB2: return CID_RGB2;
	case SSPP_DMA0: return CID_DMA0_Y + plane;
	case SSPP_DMA1: return CID_DMA1_Y + plane;
	case SSPP_VIG3: return CID_VIG3_Y + plane;
	case SSPP_RGB3: return CID_RGB3;
	default:        return CID_UNUSED;
	}
}

static inline uint32_t mixer2flush(int lm)
{
	switch (lm) {
	case 0:  return MDP5_CTL_FLUSH_LM0;
	case 1:  return MDP5_CTL_FLUSH_LM1;
	case 2:  return MDP5_CTL_FLUSH_LM2;
	default: return 0;
	}
}

static inline uint32_t intf2err(int intf)
{
	switch (intf) {
=======
static inline uint32_t intf2err(int intf_num)
{
	switch (intf_num) {
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	case 0:  return MDP5_IRQ_INTF0_UNDER_RUN;
	case 1:  return MDP5_IRQ_INTF1_UNDER_RUN;
	case 2:  return MDP5_IRQ_INTF2_UNDER_RUN;
	case 3:  return MDP5_IRQ_INTF3_UNDER_RUN;
	default: return 0;
	}
}

<<<<<<< HEAD
static inline uint32_t intf2vblank(int intf)
{
	switch (intf) {
=======
#define GET_PING_PONG_ID(layer_mixer)	((layer_mixer == 5) ? 3 : layer_mixer)
static inline uint32_t intf2vblank(int lm, struct mdp5_interface *intf)
{
	/*
	 * In case of DSI Command Mode, the Ping Pong's read pointer IRQ
	 * acts as a Vblank signal. The Ping Pong buffer used is bound to
	 * layer mixer.
	 */

	if ((intf->type == INTF_DSI) &&
			(intf->mode == MDP5_INTF_DSI_MODE_COMMAND))
		return MDP5_IRQ_PING_PONG_0_RD_PTR << GET_PING_PONG_ID(lm);

	if (intf->type == INTF_WB)
		return MDP5_IRQ_WB_2_DONE;

	switch (intf->num) {
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
	case 0:  return MDP5_IRQ_INTF0_VSYNC;
	case 1:  return MDP5_IRQ_INTF1_VSYNC;
	case 2:  return MDP5_IRQ_INTF2_VSYNC;
	case 3:  return MDP5_IRQ_INTF3_VSYNC;
	default: return 0;
	}
}

<<<<<<< HEAD
int mdp5_disable(struct mdp5_kms *mdp5_kms);
int mdp5_enable(struct mdp5_kms *mdp5_kms);

void mdp5_set_irqmask(struct mdp_kms *mdp_kms, uint32_t irqmask);
=======
static inline uint32_t lm2ppdone(int lm)
{
	return MDP5_IRQ_PING_PONG_0_DONE << GET_PING_PONG_ID(lm);
}

int mdp5_disable(struct mdp5_kms *mdp5_kms);
int mdp5_enable(struct mdp5_kms *mdp5_kms);

void mdp5_set_irqmask(struct mdp_kms *mdp_kms, uint32_t irqmask,
		uint32_t old_irqmask);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
void mdp5_irq_preinstall(struct msm_kms *kms);
int mdp5_irq_postinstall(struct msm_kms *kms);
void mdp5_irq_uninstall(struct msm_kms *kms);
irqreturn_t mdp5_irq(struct msm_kms *kms);
int mdp5_enable_vblank(struct msm_kms *kms, struct drm_crtc *crtc);
void mdp5_disable_vblank(struct msm_kms *kms, struct drm_crtc *crtc);
<<<<<<< HEAD

static inline
uint32_t mdp5_get_formats(enum mdp5_pipe pipe, uint32_t *pixel_formats,
		uint32_t max_formats)
{
	/* TODO when we have YUV, we need to filter supported formats
	 * based on pipe id..
	 */
	return mdp_get_formats(pixel_formats, max_formats);
}

void mdp5_plane_install_properties(struct drm_plane *plane,
		struct drm_mode_object *obj);
void mdp5_plane_set_scanout(struct drm_plane *plane,
		struct drm_framebuffer *fb);
int mdp5_plane_mode_set(struct drm_plane *plane,
		struct drm_crtc *crtc, struct drm_framebuffer *fb,
		int crtc_x, int crtc_y,
		unsigned int crtc_w, unsigned int crtc_h,
		uint32_t src_x, uint32_t src_y,
		uint32_t src_w, uint32_t src_h);
void mdp5_plane_complete_flip(struct drm_plane *plane);
enum mdp5_pipe mdp5_plane_pipe(struct drm_plane *plane);
struct drm_plane *mdp5_plane_init(struct drm_device *dev,
		enum mdp5_pipe pipe, bool private_plane);

uint32_t mdp5_crtc_vblank(struct drm_crtc *crtc);

void mdp5_crtc_cancel_pending_flip(struct drm_crtc *crtc, struct drm_file *file);
void mdp5_crtc_set_intf(struct drm_crtc *crtc, int intf,
		enum mdp5_intf intf_id);
void mdp5_crtc_attach(struct drm_crtc *crtc, struct drm_plane *plane);
void mdp5_crtc_detach(struct drm_crtc *crtc, struct drm_plane *plane);
struct drm_crtc *mdp5_crtc_init(struct drm_device *dev,
		struct drm_plane *plane, int id);

struct drm_encoder *mdp5_encoder_init(struct drm_device *dev, int intf,
		enum mdp5_intf intf_id);
=======
int mdp5_irq_domain_init(struct mdp5_kms *mdp5_kms);
void mdp5_irq_domain_fini(struct mdp5_kms *mdp5_kms);

uint32_t mdp5_plane_get_flush(struct drm_plane *plane);
void mdp5_plane_complete_flip(struct drm_plane *plane);
void mdp5_plane_complete_commit(struct drm_plane *plane,
	struct drm_plane_state *state);
enum mdp5_pipe mdp5_plane_pipe(struct drm_plane *plane);
struct drm_plane *mdp5_plane_init(struct drm_device *dev,
		enum mdp5_pipe pipe, bool private_plane,
		uint32_t reg_offset, uint32_t caps);

uint32_t mdp5_crtc_vblank(struct drm_crtc *crtc);

int mdp5_crtc_get_lm(struct drm_crtc *crtc);
void mdp5_crtc_cancel_pending_flip(struct drm_crtc *crtc, struct drm_file *file);
void mdp5_crtc_set_pipeline(struct drm_crtc *crtc,
		struct mdp5_interface *intf, struct mdp5_ctl *ctl);
void mdp5_crtc_wait_for_commit_done(struct drm_crtc *crtc);
struct drm_crtc *mdp5_crtc_init(struct drm_device *dev,
		struct drm_plane *plane, int id);

struct drm_encoder *mdp5_encoder_init(struct drm_device *dev,
		struct mdp5_interface *intf, struct mdp5_ctl *ctl);
int mdp5_encoder_set_split_display(struct drm_encoder *encoder,
					struct drm_encoder *slave_encoder);

#ifdef CONFIG_DRM_MSM_DSI
struct drm_encoder *mdp5_cmd_encoder_init(struct drm_device *dev,
		struct mdp5_interface *intf, struct mdp5_ctl *ctl);
int mdp5_cmd_encoder_set_split_display(struct drm_encoder *encoder,
					struct drm_encoder *slave_encoder);
#else
static inline struct drm_encoder *mdp5_cmd_encoder_init(struct drm_device *dev,
		struct mdp5_interface *intf, struct mdp5_ctl *ctl)
{
	return ERR_PTR(-EINVAL);
}
static inline int mdp5_cmd_encoder_set_split_display(
	struct drm_encoder *encoder, struct drm_encoder *slave_encoder)
{
	return -EINVAL;
}
#endif
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

#endif /* __MDP5_KMS_H__ */
