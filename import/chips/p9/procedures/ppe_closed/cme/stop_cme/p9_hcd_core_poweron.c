/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: import/chips/p9/procedures/ppe_closed/cme/stop_cme/p9_hcd_core_poweron.c $ */
/*                                                                        */
/* OpenPOWER HCODE Project                                                */
/*                                                                        */
/* COPYRIGHT 2015,2018                                                    */
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

#include "p9_cme_stop_exit_marks.h"

inline __attribute__((always_inline))
void
p9_hcd_core_poweron(uint32_t core)
{
    PK_TRACE("Assert PCB fence via NET_CTRL0[25]");
    CME_PUTSCOM(CPPM_NC0INDIR_OR, core, BIT64(25));

    PK_TRACE("Assert chiplet electrical fence via NET_CTRL0[26]");
    CME_PUTSCOM(CPPM_NC0INDIR_OR, core, BIT64(26));

    PK_TRACE("Assert vital thold via NET_CTRL0[16]");
    CME_PUTSCOM(CPPM_NC0INDIR_OR, core, BIT64(16));

    PK_TRACE("Assert core glsmux reset via PPM_CGCR[0]");
    CME_PUTSCOM(C_PPM_CGCR, core, BIT64(0));

#if !STOP_PRIME
    uint64_t scom_data = 0;

    // vdd_pfet_val/sel_override     = 0 (disbaled)
    // vdd_pfet_regulation_finger_en = 0 (controled by FSM)
    PK_TRACE("Prepare PFET Controls");
    CME_PUTSCOM(PPM_PFCS_CLR, core, BIT64(4) | BIT64(5) | BIT64(8));

    // vdd_pfet_force_state = 11 (Force Von)
    PK_TRACE("Power On Core VDD");
    CME_PUTSCOM(PPM_PFCS_OR, core, BITS64(0, 2));

    PK_TRACE("Poll for vdd_pfets_enabled_sense");

    do
    {
        CME_GETSCOM_AND(PPM_PFSNS, core, scom_data);
    }
    while(!(scom_data & BIT64(0)));

    // vdd_pfet_force_state = 00 (Nop)
    PK_TRACE("Turn Off Force Von");
    CME_PUTSCOM(PPM_PFCS_CLR, core, BITS64(0, 2));
#endif
}
