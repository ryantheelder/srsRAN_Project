/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/mac/mac_configuration_helpers.h"

using namespace srsgnb;

sched_ue_creation_request_message
srsgnb::make_scheduler_ue_creation_request(const mac_ue_create_request_message& request)
{
  sched_ue_creation_request_message ret{};
  ret.ue_index      = request.ue_index;
  ret.crnti         = request.crnti;
  ret.pcell_index   = request.cell_index;
  ret.serv_cell_cfg = request.serv_cell_cfg;

  ret.serv_cell_cfg.ul_config.emplace();
  ret.serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.emplace();
  auto& pucch_cfg = ret.serv_cell_cfg.ul_config.value().init_ul_bwp.pucch_cfg.value();

  // Configure PUCCH.
  pucch_cfg.pucch_res_set_0.pucch_res_id_list.emplace_back(0);
  pucch_cfg.pucch_res_set_0.pucch_res_id_list.emplace_back(1);

  // PUCCH resource 0.
  pucch_resource res_basic{.res_id = 0, .starting_prb = 51, .second_hop_prb = 0, .intraslot_freq_hopping = true};
  res_basic.format_1.initial_cyclic_shift = 0;
  res_basic.format_1.nof_symbols          = 14;
  res_basic.format_1.starting_sym_idx     = 0;
  res_basic.format_1.time_domain_occ      = 0;
  pucch_cfg.pucch_res_list.push_back(res_basic);

  // PUCCH resource 1.
  pucch_cfg.pucch_res_list.push_back(res_basic);
  pucch_resource& res1               = pucch_cfg.pucch_res_list.back();
  res1.format_1.initial_cyclic_shift = 4;

  // TODO: add more PUCCH resources.

  pucch_cfg.dl_data_to_ul_ack.push_back(4);

  // SR Resource
  pucch_cfg.sr_res_list.push_back(scheduling_request_resource_config{.sr_res_id    = 1,
                                                                     .sr_id        = uint_to_sched_req_id(0),
                                                                     .period       = sr_periodicity::sl_40,
                                                                     .offset       = 0,
                                                                     .pucch_res_id = 40});

  return ret;
}
