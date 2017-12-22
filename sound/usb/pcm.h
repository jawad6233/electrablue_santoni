#ifndef __USBAUDIO_PCM_H
#define __USBAUDIO_PCM_H

snd_pcm_uframes_t snd_usb_pcm_delay(struct snd_usb_substream *subs,
				    unsigned int rate);

void snd_usb_set_pcm_ops(struct snd_pcm *pcm, int stream);

int snd_usb_init_pitch(struct snd_usb_audio *chip, int iface,
		       struct usb_host_interface *alts,
		       struct audioformat *fmt);
<<<<<<< HEAD
int snd_usb_enable_audio_stream(struct snd_usb_substream *subs,
	bool enable);
=======

>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

#endif /* __USBAUDIO_PCM_H */
