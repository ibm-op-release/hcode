# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: import/chips/p9/common/pmlib/occlib/Makefile $
#
# OpenPOWER HCODE Project
#
# COPYRIGHT 2015,2017
# [+] International Business Machines Corp.
#
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied. See the License for the specific language governing
# permissions and limitations under the License.
#
# IBM_PROLOG_END_TAG
#  This Makefile currently builds a single archive, 'libocc.a', from
#  various library source files. 
#
#  part of the complete application build.
#

#all generated files from this makefile will end up in obj/$(IMAGE_NAME)/occ
export SUB_OBJDIR = /occlib

include img_defs.mk
include liboccfiles.mk

OBJS := $(addprefix $(OBJDIR)/, $(LIBOCC_OBJECTS))

libocc.a: local
	$(AR) crs $(OBJDIR)/libocc.a $(OBJDIR)/*.o

.PHONY: clean

local: $(OBJS)

$(OBJS) $(OBJS:.o=.d): | $(OBJDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -fr $(OBJDIR)

ifneq ($(MAKECMDGOALS),clean)
include $(OBJS:.o=.d) 
endif
