#
# Makefile for ALSA
#

snd-usb-audio-objs := 	card.o \
			clock.o \
			endpoint.o \
			format.o \
			helper.o \
			mixer.o \
			mixer_quirks.o \
			pcm.o \
			proc.o \
			quirks.o \
			stream.o

snd-usbmidi-lib-objs := midi.o

# Toplevel Module Dependency
obj-$(CONFIG_SND_USB_AUDIO) += snd-usb-audio.o snd-usbmidi-lib.o

obj-$(CONFIG_SND_USB_UA101) += snd-usbmidi-lib.o
obj-$(CONFIG_SND_USB_USX2Y) += snd-usbmidi-lib.o
obj-$(CONFIG_SND_USB_US122L) += snd-usbmidi-lib.o

obj-$(CONFIG_SND) += misc/ usx2y/ caiaq/ 6fire/ hiface/ bcd2000/
<<<<<<< HEAD
obj-$(CONFIG_SND_USB_AUDIO_QMI) += usb_audio_qmi_v01.o usb_audio_qmi_svc.o
=======
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24
