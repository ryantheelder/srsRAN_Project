/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../../common/f1ap_asn1_utils.h"
#include "f1ap_du_event_manager.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/f1c/du/f1ap_du.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
namespace srs_du {

struct f1ap_du_context;

/// F1 Setup Procedure for the gNB-DU as per TS 38.473, 8.2.3.
class f1ap_du_setup_procedure
{
public:
  f1ap_du_setup_procedure(const f1_setup_request_message& request_,
                          f1c_message_notifier&           cu_notif_,
                          f1ap_event_manager&             ev_mng_,
                          timer_manager&                  timers,
                          f1ap_du_context&                du_ctxt_);

  void operator()(coro_context<async_task<f1_setup_response_message>>& ctx);

private:
  /// Send F1 SETUP REQUEST to CU.
  void send_f1_setup_request();

  /// Checks whether the DU should attempt again to connect to CU.
  bool retry_required();

  /// Creates procedure result to send back to procedure caller.
  f1_setup_response_message create_f1_setup_result();

  const f1_setup_request_message request;
  f1c_message_notifier&          cu_notifier;
  f1ap_event_manager&            ev_mng;
  srslog::basic_logger&          logger;
  f1ap_du_context&               du_ctxt;

  unique_timer f1_setup_wait_timer;

  f1ap_transaction transaction;
  unsigned         f1_setup_retry_no = 0;
  unsigned         time_to_wait      = 0;
}; // namespace srs_du

} // namespace srs_du
} // namespace srsgnb