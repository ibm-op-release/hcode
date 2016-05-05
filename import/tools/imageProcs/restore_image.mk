# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: import/tools/imageProcs/restore_image.mk $
#
# OpenPOWER HCODE Project
#
# COPYRIGHT 2016,2017
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
IMAGE=restore_image

# create dependency on the restore image hcode and the final step in the
# raw image.bin creation
SELF_REST_TARGET=$(ROOTPATH)/chips/p9/procedures/utils/stopreg/selfRest.bin

SELF_REST_DEPS=$$($(IMAGE)_PATH)/.$(IMAGE).setbuild_user

$(call XIP_TOOL,append,.self_restore,$(SELF_REST_DEPS),$(SELF_REST_TARGET))
$(call XIP_TOOL,report,,$$($(IMAGE)_PATH)/.$(IMAGE).append.self_restore,,)
$(call BUILD_XIPIMAGE)
