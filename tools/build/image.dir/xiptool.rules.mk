# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: tools/build/image.dir/xiptool.rules.mk $
#
# IBM CONFIDENTIAL
#
# EKB Project
#
# COPYRIGHT 2016,2017
# [+] International Business Machines Corp.
#
#
# The source code for this program is not published or otherwise
# divested of its trade secrets, irrespective of what has been
# deposited with the U.S. Copyright Office.
#
# IBM_PROLOG_END_TAG

# Makefile that automatically define the recipies for all the p9_xip_tool
# commands
#
# Macro:
#    XIP_TOOL: Defines a generic recipie to run any xip_tool command
# Input:
#    $1 <- XIP Command
#    $2 <- section name <optional>
#    $3 <- any option that we need to pass to the XIP command and NEED to set a
#          dependency on <optional>
#    $4 <- any options that needs to be passed to the XIP command that we don't
#          need to set a dependency on <optional>
#    $5 <- optional target file diferentiator (eg. ec level)
# Usage: $(call XIP_TOOL, append, .sgpe, path/to/sgpe.bin)

XIP_TOOL = $(eval $(call _XIP_TOOL,$1,$2,$3,$4,$5))

define _XIP_TOOL
$(IMAGE)_PATH ?= $(IMAGEPATH)/$(IMAGE)

$$($(IMAGE)_PATH)/.$(IMAGE).$(1)$(2)$(5) : $(EXEPATH)/p9_xip_tool.exe $3
		$(C2) "    GEN        $$(@F)"
		$(C1)  $(EXEPATH)/p9_xip_tool.exe $$($(IMAGE)_PATH)/$(IMAGE).bin \
			$1 $2 $4 > $$($(IMAGE)_PATH)/$(IMAGE).$(1)$(2)$(5) && touch $$@


$(IMAGE)_TARGETS+= $$($(IMAGE)_PATH)/.$(IMAGE).$(1)$(2)$(5)
$(call __CLEAN_TARGET,$$($(IMAGE)_PATH)/.$(IMAGE).$(1)$(2)$(5))
$(call __CLEAN_TARGET,$$($(IMAGE)_PATH)/$(IMAGE).$(1)$(2)$(5))
$(call __CLEAN_TARGET,$$@)

endef

# XIP_NORMALIZE command needs to be executed prior to generating a bin file for
# an image. However, all the other xip commonads are executed after the
# generation of the bin file. Therefore, we have a separate macro for XIP
# NORMALIZE. If we use the generic XIP_TOOL, then we will end up with circular
# dependency
define XIP_NORMALIZE
	$(C1) $(EXEPATH)/p9_xip_tool.exe $1  normalize
endef

# Macros:
#    APPEND_EMPTY_SECTION: Creates an image of a given size and appends it to
#    $(IMAGE).bin
# Input:
#    $1 == XIP section to append the image to
#    $2 == Size of the section (in bytes)
#    $3 == Optional dependancy file
# Usage:
#    $(call APPEND_EMPTY_SECTION,hcode,1024,$(CME_IMAGE_DEPS))
APPEND_EMPTY_SECTION = $(eval $(call _APPEND_EMPTY_SECTION,$1,$2,$3))

# Order of Operation:
#    - Define default path to the image being genreated
#    - Create a rule to generate an image of all zeros with a given size
#    - call XIP_TOOL append command to append the generated section to
#      the image
define _APPEND_EMPTY_SECTION
$(IMAGE)_PATH ?= $(IMAGEPATH)/$(IMAGE)

$$($(IMAGE)_PATH)/$1.bin : $$($(IMAGE)_PATH)/.$(IMAGE).normalize.bin.built $3
		$(C2) "    GEN        $$(@F)"
		$(C1) dd if=/dev/zero of=$$($(IMAGE)_PATH)/$1.bin count=1 bs=$2 && \
		touch $$($(IMAGE)_PATH)/$1.bin.done

$$($(IMAGE)_PATH)/$1.bin.done: $$($(IMAGE)_PATH)/$1.bin

$(call XIP_TOOL,append,.$(1),$$($(IMAGE)_PATH)/$1.bin.done,$$($(IMAGE)_PATH)/$1.bin)

$(call __CLEAN_TARGET, $$($(IMAGE)_PATH)/$1.bin.done)
$(call __CLEAN_TARGET, $$($(IMAGE)_PATH)/$1.bin)
endef
