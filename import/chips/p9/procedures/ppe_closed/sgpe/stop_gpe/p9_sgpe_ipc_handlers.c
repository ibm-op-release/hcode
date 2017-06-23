/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: import/chips/p9/procedures/ppe_closed/sgpe/stop_gpe/p9_sgpe_ipc_handlers.c $ */
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

#include "p9_sgpe_stop.h"
#include "p9_sgpe_irq.h"

extern SgpeStopRecord G_sgpe_stop_record;


void
p9_sgpe_ipc_pgpe_ctrl_stop_updates(ipc_msg_t* cmd, void* arg)
{
    PkMachineContext ctx;

    ipc_async_cmd_t* async_cmd = (ipc_async_cmd_t*)cmd;
    ipcmsg_p2s_ctrl_stop_updates_t* msg =
        (ipcmsg_p2s_ctrl_stop_updates_t*)async_cmd->cmd_data;

    msg->fields.return_code  = IPC_SGPE_PGPE_RC_SUCCESS;
    msg->fields.active_quads = G_sgpe_stop_record.group.quad[VECTOR_ACTIVE] >> SHIFT32(5);
    msg->fields.active_cores = G_sgpe_stop_record.group.core[VECTOR_ACTIVE] >> SHIFT32(23);

    if (msg->fields.action < 4)
    {
        //enable:  core = 0b001, quad = 0b010, both = 0b011
        G_sgpe_stop_record.wof.update_pgpe |= msg->fields.action;
    }
    else
    {
        //disable: core = 0b101, quad = 0b110, both = 0b111
        G_sgpe_stop_record.wof.update_pgpe &= ~(msg->fields.action);
    }

    ipc_send_rsp(cmd, IPC_RC_SUCCESS);

    pk_irq_vec_restore(&ctx);
}

void
p9_sgpe_ipc_pgpe_suspend_stop(ipc_msg_t* cmd, void* arg)
{
    PkMachineContext ctx;

    G_sgpe_stop_record.wof.suspend_cmd = cmd;
    g_oimr_override |= (BITS64(47, 2) | BIT64(51));

    // stop in process
    if (G_sgpe_stop_record.wof.status_stop == STATUS_PROCESSING)
    {
        // Note: response will be sent by stop threads when suspension is completed
        G_sgpe_stop_record.wof.status_stop = STATUS_SUSPENDING;
    }
    // sgpe idle
    else if (G_sgpe_stop_record.wof.status_stop == STATUS_IDLE)
    {
        p9_sgpe_stop_suspend_all_cmes();
    }

    pk_irq_vec_restore(&ctx);
}

void
p9_sgpe_stop_suspend_all_cmes()
{
    uint32_t qloop       = 0;
    uint32_t xloop       = 0;
    uint32_t cloop       = 0;
    uint32_t cpayload_t2 = 0;
    uint32_t cme_list    = 0;

    for(qloop = 0; qloop < MAX_QUADS; qloop++)
    {
        p9_sgpe_stop_suspend_msg_db1(qloop, BITS32(5, 3));
    }

    while (cme_list != 0xFFF00000)
    {
        for(xloop = 0; xloop < MAX_EXES; xloop++)
        {
            for(cloop = 0; cloop < CORES_PER_EX; cloop++)
            {
                cpayload_t2 = in32(OCB_OPIT2CN(((xloop << 1) + cloop)));

                if (cpayload_t2 == 0x780)
                {
                    cme_list |= BIT32(xloop);
                }
            }
        }
    }

    ipc_async_cmd_t* async_cmd =
        (ipc_async_cmd_t*)(G_sgpe_stop_record.wof.suspend_cmd);
    ipcmsg_p2s_suspend_stop_t* msg =
        (ipcmsg_p2s_suspend_stop_t*)async_cmd->cmd_data;
    msg->fields.return_code = IPC_SGPE_PGPE_RC_SUCCESS;
    ipc_send_rsp(G_sgpe_stop_record.wof.suspend_cmd, IPC_RC_SUCCESS);
    G_sgpe_stop_record.wof.status_stop = STATUS_SUSPENDED;
}
