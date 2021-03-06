# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: tools/build/image.dir/bin.rules.mk $
#
# IBM CONFIDENTIAL
#
# EKB Project
#
# COPYRIGHT 2016
# [+] International Business Machines Corp.
#
#
# The source code for this program is not published or otherwise
# divested of its trade secrets, irrespective of what has been
# deposited with the U.S. Copyright Office.
#
# IBM_PROLOG_END_TAG

# Makefile that defines how we generate binaries for a specific XIP image
#
# Macros:
#    GEN_IMAGE_BINARY: Macro to generate an XIP normalized binary and
#                      disassembly from an elf executable
# Input:
#    $1 == IMAGE
#    $2 == OUTPUT PATH
#    $3 == TOOLCHAIN
# Order Of Operation:
#    Run objcopy and objdump to gnerate an image_temp.bin
#    Run p9_xip_tool's normalize commonad on image_temp.bin
#    After normalization, copy image_temp.bin to image.bin
#    Set build_date, build_time, build_user, and build_host information in the
#        image
#    Create a report for this image
define GEN_IMAGE_BINARY
$2/$1_temp.bin : $2/$1.out
		$(C2) "    GEN        $$(@F)"
		$(C1) $$($(3)_PREFIX)$(OBJCOPY) -O binary $$^ $$@ --gap-fill 0x00 --pad-to 0x`/usr/bin/nm $$^ \
		| grep "image_end" | cut -d " " -f 1`
		$(C2) "    GEN        $$(@F).dis"
		$(C1) $$($(3)_PREFIX)$$(OBJDUMP) -S $$^ > $2/$$(@F).dis

$2/.$1.normalize.bin.built : $2/$1_temp.bin $(EXEPATH)/p9_xip_tool.exe
		$(C2) "    GEN        $$(@F)"
		$(call XIP_NORMALIZE,$2/$1_temp.bin) && \
		cp $2/$1_temp.bin $2/$1.bin && touch $$@

$(call XIP_TOOL,set,build_date,$2/.$1.normalize.bin.built,`date +%Y%m%d`)
$(call XIP_TOOL,set,build_time,$2/.$1.setbuild_date,`date +%H%M`)
$(call XIP_TOOL,set,build_user,$2/.$1.setbuild_time,`id -un`)
$(call XIP_TOOL,set,build_host,$2/.$1.setbuild_user,`hostname`)

$2/.$1.bin.built : $2/.$1.normalize.bin.built $$($1_TARGETS)
		$(C2) "    GEN        $$(@F)"
		$(C1) touch $$@

$2/$1.bin : $2/.$1.bin.built
$2/$1.dis : $2/.$1.bin.built


$(call __CLEAN_TARGET,$2/.$1.bin.built)
$(call __CLEAN_TARGET,$2/.$1.normalize.bin.built)
$(call __CLEAN_TARGET,$2/$1_temp.bin)
$(call __CLEAN_TARGET,$2/$1.bin)
$(call __CLEAN_TARGET,$2/$1.s)
$(call __CLEAN_TARGET,$2/$1.o)
$(call __CLEAN_TARGET,$2/$1.dis)
$(call __CLEAN_TARGET,$2/$1_temp.bin.dis)
endef

