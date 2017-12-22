KASAN_SANITIZE := n
<<<<<<< HEAD
=======
UBSAN_SANITIZE_kasan.o := n
KCOV_INSTRUMENT := n
>>>>>>> 8f5d770414a10b7c363c32d12f188bd16f7b6f24

CFLAGS_REMOVE_kasan.o = -pg
# Function splitter causes unnecessary splits in __asan_load1/__asan_store1
# see: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=63533
CFLAGS_kasan.o := $(call cc-option, -fno-conserve-stack -fno-stack-protector)

obj-y := kasan.o report.o kasan_init.o
