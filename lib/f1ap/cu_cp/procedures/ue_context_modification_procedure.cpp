/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_context_modification_procedure.h"
#include "../../common/asn1_helpers.h"
#include "../f1ap_asn1_helpers.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/ran/cause.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;
using namespace asn1::f1ap;

ue_context_modification_procedure::ue_context_modification_procedure(
    const f1ap_ue_context_modification_request& request_,
    f1ap_ue_context&                            ue_ctxt_,
    f1ap_message_notifier&                      f1ap_notif_,
    srslog::basic_logger&                       logger_) :
  request(request_), ue_ctxt(ue_ctxt_), f1ap_notifier(f1ap_notif_), logger(logger_)
{
}

void ue_context_modification_procedure::operator()(coro_context<async_task<f1ap_ue_context_modification_response>>& ctx)
{
  CORO_BEGIN(ctx);

  logger.debug("ue={}: \"{}\" initialized.", ue_ctxt.ue_index, name());

  // Subscribe to respective publisher to receive UE CONTEXT MODIFICATION RESPONSE/FAILURE message.
  transaction_sink.subscribe_to(ue_ctxt.ev_mng.context_modification_outcome);

  // Send command to DU.
  send_ue_context_modification_request();

  // Await CU response.
  CORO_AWAIT(transaction_sink);

  // Handle response from DU and return UE index
  CORO_RETURN(create_ue_context_modification_result());
}

void ue_context_modification_procedure::send_ue_context_modification_request()
{
  // Pack message into PDU
  f1ap_message f1ap_ue_ctxt_mod_request_msg;
  f1ap_ue_ctxt_mod_request_msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_MOD);
  ue_context_mod_request_s& ctx_mod = f1ap_ue_ctxt_mod_request_msg.pdu.init_msg().value.ue_context_mod_request();

  fill_asn1_ue_context_modification_request(ctx_mod, request);

  ctx_mod->gnb_du_ue_f1ap_id = gnb_du_ue_f1ap_id_to_uint(ue_ctxt.du_ue_f1ap_id);
  ctx_mod->gnb_cu_ue_f1ap_id = gnb_cu_ue_f1ap_id_to_uint(ue_ctxt.cu_ue_f1ap_id);

  if (logger.debug.enabled()) {
    asn1::json_writer js;
    f1ap_ue_ctxt_mod_request_msg.pdu.to_json(js);
    logger.debug("Containerized UeContextModificationRequest: {}", js.to_string());
  }

  // send UE context modification request message
  f1ap_notifier.on_new_message(f1ap_ue_ctxt_mod_request_msg);
}

f1ap_ue_context_modification_response ue_context_modification_procedure::create_ue_context_modification_result()
{
  f1ap_ue_context_modification_response res{};

  if (transaction_sink.successful()) {
    const asn1::f1ap::ue_context_mod_resp_s& resp = transaction_sink.response();
    logger.debug("Received UeContextModificationResponse");
    if (logger.debug.enabled()) {
      asn1::json_writer js;
      resp.to_json(js);
      logger.debug("Containerized UeContextModificationResponse: {}", js.to_string());
    }
    fill_f1ap_ue_context_modification_response(res, resp);

    logger.debug("ue={}: \"{}\" finalized.", ue_ctxt.ue_index, name());
  } else if (transaction_sink.failed()) {
    const asn1::f1ap::ue_context_mod_fail_s& fail = transaction_sink.failure();
    logger.debug("Received UeContextModificationFailure cause={}", get_cause_str(fail->cause));
    if (logger.debug.enabled()) {
      asn1::json_writer js;
      (*transaction_sink.failure()).to_json(js);
      logger.debug("Containerized UeContextModificationFailure: {}", js.to_string());
    }
    fill_f1ap_ue_context_modification_response(res, fail);

    logger.error("ue={}: \"{}\" failed.", ue_ctxt.ue_index, name());
  } else {
    logger.warning("UeContextModificationResponse timeout");
    res.success = false;
    res.cause   = cause_misc_t::unspecified;

    logger.error("ue={}: \"{}\" failed.", ue_ctxt.ue_index, name());
  }

  return res;
}
