# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: import/tools/imageProcs/cme_image.mk $
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
IMAGE=cme_image
$(call ADD_IMAGE_INCDIR,$(IMAGE),$(ROOTPATH)/chips/p9/xip)
$(call APPEND_EMPTY_SECTION,vect,1024)
$(call APPEND_EMPTY_SECTION,cme_image_header,1024)
$(call APPEND_EMPTY_SECTION,hcode,1024)
$(call BUILD_IMAGE)