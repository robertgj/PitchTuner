# aplay.c was downloaded with:
#   http://www.alsa-project.org/files/pub/utils/alsa-utils-1.2.8.tar.bz2

alsa_examples_PROGRAMS=capture interrupt playback query sa_aplay
PROGRAMS+=$(alsa_examples_PROGRAMS)
VPATH += src/SimpleAudio/alsa/examples

capture_C_SOURCES:=capture.c
interrupt_C_SOURCES:=interrupt.c
playback_C_SOURCES:=playback.c
sa_aplay_C_SOURCES:=aplay.c
query_C_SOURCES:=query.c

alsa_examples_C_SOURCES:=$(capture_C_SOURCES) $(interrupt_C_SOURCES) \
$(playback_C_SOURCES) $(sa_aplay_C_SOURCES) $(query_C_SOURCES)

$(call add_extra_CFLAGS_macro, $(alsa_examples_C_SOURCES), -D_POSIX_C_SOURCE)

$(call add_extra_CFLAGS_macro, $(sa_aplay_C_SOURCES), \
-Isrc/SimpleAudio/alsa/examples/include \
-Wno-format-nonliteral \
-Wno-double-promotion \
-Wno-sign-compare \
-Wno-conversion \
-Wno-sign-conversion \
-Wno-shadow \
-Wno-cast-qual \
-Wno-cast-align \
-Wno-unused-parameter )

# The following are not accepted by clang
$(call add_extra_CFLAGS_macro, $(sa_aplay_C_SOURCES), \
-Wno-old-style-declaration \
-Wno-discarded-qualifiers \
-Wno-analyzer-possible-null-argument \
-Wno-analyzer-use-of-uninitialized-value \
-Wno-analyzer-possible-null-dereference \
-Wno-analyzer-malloc-leak )

$(call add_extra_LIBS_macro, $(alsa_examples_PROGRAMS), -lasound)
