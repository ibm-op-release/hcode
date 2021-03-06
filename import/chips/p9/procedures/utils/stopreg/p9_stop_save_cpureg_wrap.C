/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: import/chips/p9/procedures/utils/stopreg/p9_stop_save_cpureg_wrap.C $ */
/*                                                                        */
/* OpenPOWER HCODE Project                                                */
/*                                                                        */
/* COPYRIGHT 2016,2017                                                    */
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
// Wrapper function for p9_stop_save_cpureg() so Cronus doesn't have to compile C code from ekb directly into Cronus exe.

#include <p9_stop_save_cpureg_wrap.H>

extern "C"
{
    fapi2::ReturnCode p9_stop_save_cpureg_wrap(void* const i_pImage, const stopImageSection::CpuReg_t i_regId,
            const uint64_t i_regData, const uint64_t i_pir)
    {
        stopImageSection::StopReturnCode_t l_rc = p9_stop_save_cpureg(i_pImage, i_regId, i_regData, i_pir);
        return fapi2::ReturnCode(l_rc);
    }


} // extern "C"
