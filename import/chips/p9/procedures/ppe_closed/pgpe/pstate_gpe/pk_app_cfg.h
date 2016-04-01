/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: import/chips/p9/procedures/ppe_closed/pgpe/pstate_gpe/pk_app_cfg.h $ */
/*                                                                        */
/* OpenPOWER HCODE Project                                                */
/*                                                                        */
/* COPYRIGHT 2015,2017                                                    */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
#ifndef __PK_APP_CFG_H__
#define __PK_APP_CFG_H__
//-----------------------------------------------------------------------------
// *! (C) Copyright International Business Machines Corp. 2014
// *! All Rights Reserved -- Property of IBM
// *! *** IBM Confidential ***
//-----------------------------------------------------------------------------

/// \file pk_app_cfg.h
/// \brief Application specific overrides go here.
///
#include "occ_app_cfg.h"
#include "global_app_cfg.h"

/// The Instance ID of the OCC processor that this application is intended to run on
///// 0-3 -> GPE, 4 -> 405
#define APPCFG_OCC_INSTANCE_ID 2

/// This application will statically initialize it's external interrupt table
/// using the table defined in pk_app_irq_table.c.
#define STATIC_IRQ_TABLE

/// About OCCHW_IRQ_PMC_PCB_INTR_TYPE1_PENDING :
/// This interrupt is used by the PGPE (GPE2) exclusively. Thus, rather than
///   defining the entire OCCHW_IRQ_ROUTES table here, and thus over-riding
///   the default routing table, I've updated the default table in
///   p9_code/include/occhw_irq_config.h .

/// Static configuration data for external interrupts:
/// Note, that these interrupts only have relevance for 405 IPC messaging and
///   xstop. The PCB type 1 interrupt is configured manually in the code.)
///
/// IRQ#, TYPE, POLARITY, ENABLE
///
#define APPCFG_EXT_IRQS_CONFIG \
    OCCHW_IRQ_CHECK_STOP_GPE2             OCCHW_IRQ_TYPE_EDGE  OCCHW_IRQ_POLARITY_RISING  OCCHW_IRQ_MASKED \
    OCCHW_IRQ_IPI2_HI_PRIORITY            OCCHW_IRQ_TYPE_EDGE  OCCHW_IRQ_POLARITY_RISING  OCCHW_IRQ_MASKED \
    OCCHW_IRQ_IPI2_LO_PRIORITY            OCCHW_IRQ_TYPE_EDGE  OCCHW_IRQ_POLARITY_RISING  OCCHW_IRQ_MASKED \
    OCCHW_IRQ_PMC_PCB_INTR_TYPE1_PENDING  OCCHW_IRQ_TYPE_LEVEL OCCHW_IRQ_POLARITY_HI      OCCHW_IRQ_MASKED \
    OCCHW_IRQ_PMC_PCB_INTR_TYPE4_PENDING  OCCHW_IRQ_TYPE_LEVEL OCCHW_IRQ_POLARITY_HI      OCCHW_IRQ_MASKED
#endif /*__PK_APP_CFG_H__*/