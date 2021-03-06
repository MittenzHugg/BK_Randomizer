target          = mips64
PACKAGE        ?= $(NAME)
URL            ?= github.com/MittenzHugg/Randjo-Kazooie
CC              = $(target)-gcc
LD              = $(target)-g++
AS              = $(target)-gcc -x assembler-with-cpp
OBJCOPY         = $(target)-objcopy
GRC             = grc
CFILES          = *.c
SFILES          = *.S
RESFILES        = *.png
SRCDIR          = src
OBJDIR          = obj
BINDIR          = bin
LIBDIR          = lib
RANDO_VERSIONS     = NBKJ NBKE NBKE11 NBKP
RESDESC         = res.json

ADDRESS         = 0x80400060
ADDRESS_LDR     = 0x80000400
ALL_CPPFLAGS    = $(CPPFLAGS)
ALL_CFLAGS      = -c -Ilibundermine/include -MMD -MP -std=gnu11 -Wall -ffunction-sections -fdata-sections -O1
ALL_LDFLAGS     = -T gl-n64.ld -L$(LIBDIR) -Llibundermine/lib/libundermine-f3dex -nostartfiles -specs=nosys.specs -Wl,--gc-sections -O1 $(LDFLAGS)
ALL_LIBS        = $(LIBS)

RANDO	            = $(foreach v,$(RANDO_VERSIONS),rando-$(v))
LDR             = $(foreach v,$(RANDO_VERSIONS),ldr-rando-$(v))

RANDO-NBKJ         = $(OBJ-rando-NBKJ) $(ELF-rando-NBKJ)
RANDO-NBKE			= $(OBJ-rando-NBKE) $(ELF-rando-NBKE)
RANDO-NBKE11		= $(OBJ-rando-NBKE11) $(ELF-rando-NBKE11)
RANDO-NBKP			= $(OBJ-rando-NBKP) $(ELF-rando-NBKP)

debug_warp           : src/rando/warp_debug.c src/rando/warps.c
	gcc -DDEBUG -o build/debug_warp src/rando/warp_debug.c src/rando/warps.c src/rando/tree.c src/rando/bitfield.c

debug_clean     :
	rm -rf debug_warp build/debug_warp

all	            : $(RANDO)
clean           :
	rm -rf $(OBJDIR) $(BINDIR)

distclean       : clean
	rm -rf build/*.z64 build/*.wad

clean-libum     :
	cd libundermine && make clean

distclean-libum :
	cd libundermine && make distclean

config-libum    : libundermine/config.status
libundermine/config.status :
	cd libundermine && ./configure \
		--host=$(target) \
        --enable-lto

libum           : libundermine/config.status
	cd libundermine && make lib/libundermine-f3dex/libundermine-f3dex.a

.PHONY          : all clean distclean clean-libum distclean-libum config-libum libum



define bin_template
SRCDIR-$(1)	    = $(5)
OBJDIR-$(1)	    = obj/$(1)
BINDIR-$(1)     = bin/$(1)
RESDIR-$(1)	    = $(6)
NAME-$(1)	    = $(1)
ADDRESS-$(1)    = $(4)
CSRC-$(1)      := $$(foreach s,$$(CFILES),$$(wildcard $$(SRCDIR-$(1))/$$(s)))
COBJ-$(1)       = $$(patsubst $$(SRCDIR-$(1))/%,$$(OBJDIR-$(1))/%.o,$$(CSRC-$(1)))
SSRC-$(1)      := $$(foreach s,$$(SFILES),$$(wildcard $$(SRCDIR-$(1))/$$(s)))
SOBJ-$(1)       = $$(patsubst $$(SRCDIR-$(1))/%,$$(OBJDIR-$(1))/%.o,$$(SSRC-$(1)))
RESSRC-$(1)    := $$(wildcard $$(RESDIR-$(1))/*)
RESOBJ-$(1)     = $$(patsubst $$(RESDIR-$(1))/%,$$(OBJDIR-$(1))/%.o,$$(RESSRC-$(1)))
OBJ-$(1)        = $$(COBJ-$(1)) $$(SOBJ-$(1)) $$(RESOBJ-$(1))
ELF-$(1)        = $$(BINDIR-$(1))/$(3).elf
BIN-$(1)        = $$(BINDIR-$(1))/$(3).bin
OUTDIR-$(1)     = $$(OBJDIR-$(1)) $$(BINDIR-$(1))
BUILD-$(1)      = $(1)
CLEAN-$(1)      = clean-$(1)
$$(ELF-$(1))    : LDFLAGS += -Wl,--defsym,start=$$(ADDRESS-$(1))
$$(BUILD-$(1))  : $$(BIN-$(1))
$$(CLEAN-$(1))  :
	rm -rf $$(OUTDIR-$(1))
$$(COBJ-$(1))   : $$(OBJDIR-$(1))/%.o: $$(SRCDIR-$(1))/% | $$(OBJDIR-$(1))
	$(CC) $$(ALL_CPPFLAGS) $$(ALL_CFLAGS) $$< -o $$@
$$(SOBJ-$(1))   : $$(OBJDIR-$(1))/%.o: $$(SRCDIR-$(1))/% | $$(OBJDIR-$(1))
	$(AS) -c -MMD -MP $$(ALL_CPPFLAGS) $$< -o $$@
$$(RESOBJ-$(1)) : $$(OBJDIR-$(1))/%.o: $$(RESDIR-$(1))/% | $$(OBJDIR-$(1))
	$(GRC) $$< -d $(RESDESC) -o $$@
$$(ELF-$(1))    : $$(OBJ-$(1)) | $$(BINDIR-$(1))
	$(LD) $$(ALL_LDFLAGS) $$^ $$(ALL_LIBS) -o $$@
$$(BIN-$(1))    : $$(ELF-$(1)) | $$(BINDIR-$(1))
	$(OBJCOPY) -S -O binary $$< $$@
$$(OUTDIR-$(1)) :
	mkdir -p $$@
endef

$(foreach v,$(RANDO_VERSIONS),$(eval $(call bin_template,rando-$(v),$(v),rando,$(ADDRESS),src/rando,res)))
$(foreach v,$(RANDO_VERSIONS),$(eval $(call bin_template,ldr-rando-$(v),$(v),ldr,$(ADDRESS_LDR),src/ldr,res/ldr)))

$(RANDO-NBKJ)      : LIBS := -lnbkj -Wl,--whole-archive -lundermine-f3dex -Wl,--no-whole-archive
$(RANDO-NBKE)      : LIBS := -lnbke -Wl,--whole-archive -lundermine-f3dex -Wl,--no-whole-archive
$(RANDO-NBKE11)    : LIBS := -lnbke11 -Wl,--whole-archive -lundermine-f3dex -Wl,--no-whole-archive
$(RANDO-NBKP)      : LIBS := -lnbkp -Wl,--whole-archive -lundermine-f3dex -Wl,--no-whole-archive