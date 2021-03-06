# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: import/chips/p9/common/pmlib/occlib/liboccfiles.mk $
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
#  @file liboccfiles.mk
#
#  @brief mk for libocc.a object files
#
#  @page ChangeLogs Change Logs
#  @section liboccfiles.mk
#  @verbatim
#
#
# Change Log ******************************************************************
# Flag     Defect/Feature  User        Date         Description
# ------   --------------  ----------  ------------ -----------
#
# @endverbatim
#
##########################################################################
# INCLUDES
##########################################################################

C-SOURCES = \
    ipc_core.c \
	ipc_init.c \
	ipc_msgq.c \
	ipc_ping.c \
	occhw_xir_dump.c

S-SOURCES =  

LIBOCC_OBJECTS = $(C-SOURCES:.c=.o) $(S-SOURCES:.S=.o)
