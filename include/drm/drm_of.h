#ifndef __DRM_OF_H__
#define __DRM_OF_H__

<<<<<<< HEAD
=======
struct component_master_ops;
struct device;
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
struct drm_device;
struct device_node;

#ifdef CONFIG_OF
extern uint32_t drm_of_find_possible_crtcs(struct drm_device *dev,
					   struct device_node *port);
<<<<<<< HEAD
=======
extern int drm_of_component_probe(struct device *dev,
				  int (*compare_of)(struct device *, void *),
				  const struct component_master_ops *m_ops);
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
#else
static inline uint32_t drm_of_find_possible_crtcs(struct drm_device *dev,
						  struct device_node *port)
{
	return 0;
}
<<<<<<< HEAD
=======

static inline int
drm_of_component_probe(struct device *dev,
		       int (*compare_of)(struct device *, void *),
		       const struct component_master_ops *m_ops)
{
	return -EINVAL;
}
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
#endif

#endif /* __DRM_OF_H__ */
