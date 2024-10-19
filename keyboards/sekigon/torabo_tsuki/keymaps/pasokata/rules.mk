VIA_ENABLE = yes
VIAL_ENABLE = yes
VIAL_INSECURE = yes

$(INTERMEDIATE_OUTPUT)/quantum/qmk_settings.o:: CFLAGS += -Dget_hold_on_other_key_press=get_hold_on_other_key_press_vial


OS_DETECTION_ENABLE = yes
SRC+=oneshot.c
SRC+=swapper.c
