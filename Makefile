CONTIKI_PROJECT = secretKnockDetector
all: $(CONTIKI_PROJECT)

CONTIKI = ../../contiki-ng
MAKE_NET = MAKE_NET_NULLNET
include $(CONTIKI)/Makefile.include
