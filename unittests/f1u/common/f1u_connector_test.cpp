/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "apps/gnb/adapters/f1u_connector/f1u_local_connector.h"
#include "srsgnb/srslog/srslog.h"
#include <gtest/gtest.h>

using namespace srsgnb;

// dummy CU-UP RX bearer interface
struct dummy_f1u_cu_up_rx_sdu_notifier final : public srs_cu_up::f1u_rx_sdu_notifier {
  void on_new_sdu(byte_buffer_slice_chain sdu) override
  {
    logger.info(sdu.begin(), sdu.end(), "CU-UP received SDU");
    last_sdu = std::move(sdu);
  }
  byte_buffer_slice_chain last_sdu;
  srslog::basic_logger&   logger = srslog::fetch_basic_logger("F1-U", false);
};

struct dummy_f1u_cu_up_rx_delivery_notifier final : public srs_cu_up::f1u_rx_delivery_notifier {
  void on_delivered_sdu(uint32_t count) override { logger.info("CU-UP PDU with COUNT={} delivered", count); }
  srslog::basic_logger& logger = srslog::fetch_basic_logger("F1-U", false);
};

// dummy DU RX bearer interface
struct dummy_f1u_du_rx_sdu_notifier final : public srs_du::f1u_rx_sdu_notifier {
  void on_new_sdu(pdcp_tx_pdu sdu) override
  {
    logger.info(sdu.buf.begin(), sdu.buf.end(), "DU received SDU. COUNT={}", sdu.pdcp_count);
    last_sdu = std::move(sdu.buf);
  }
  void        on_discard_sdu(uint32_t count) override {}
  byte_buffer last_sdu;

private:
  srslog::basic_logger& logger = srslog::fetch_basic_logger("F1-U", false);
};

/// Fixture class for F1-U connector tests.
/// It requires TEST_F() to create/spawn tests
class f1u_connector_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // init test's logger
    srslog::init();
    logger.set_level(srslog::basic_levels::debug);

    // init logger
    f1u_logger.set_level(srslog::basic_levels::debug);
    f1u_logger.set_hex_dump_max_size(100);

    logger.info("Creating F1-U connector");

    // create f1-u connector
    f1u_conn = std::make_unique<f1u_local_connector>();
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  std::unique_ptr<f1u_local_connector> f1u_conn;
  srslog::basic_logger&                logger     = srslog::fetch_basic_logger("TEST", false);
  srslog::basic_logger&                f1u_logger = srslog::fetch_basic_logger("F1-U", false);
};

/// Test the instantiation of a new entity
TEST_F(f1u_connector_test, create_new_connector)
{
  EXPECT_NE(f1u_conn, nullptr);
  EXPECT_NE(f1u_conn->get_f1u_du_gateway(), nullptr);
  EXPECT_NE(f1u_conn->get_f1u_cu_up_gateway(), nullptr);
}

/// Test attaching F1-U bearer at CU-UP and DU
TEST_F(f1u_connector_test, attach_cu_up_f1u_to_du_f1u)
{
  f1u_cu_up_gateway* cu_gw = f1u_conn->get_f1u_cu_up_gateway();
  f1u_du_gateway*    du_gw = f1u_conn->get_f1u_du_gateway();

  // Create CU TX notifier adapter
  dummy_f1u_cu_up_rx_sdu_notifier      cu_rx;
  dummy_f1u_cu_up_rx_delivery_notifier cu_delivery;
  srs_cu_up::f1u_bearer*               cu_bearer = cu_gw->create_cu_dl_bearer(1, cu_delivery, cu_rx);

  // Create DU TX notifier adapter and RX handler
  dummy_f1u_du_rx_sdu_notifier du_rx;
  srs_du::f1u_bearer*          du_bearer = du_gw->create_du_ul_bearer(1, 2, du_rx);

  // Create CU RX handler and attach it to the DU TX
  cu_gw->attach_cu_ul_bearer(1, 2);

  // Check CU-UP -> DU path
  byte_buffer             cu_buf = make_byte_buffer("ABCD");
  byte_buffer_slice_chain du_exp{cu_buf.deep_copy()};
  pdcp_tx_pdu             sdu;
  sdu.buf            = std::move(cu_buf);
  sdu.has_pdcp_count = true;
  sdu.pdcp_count     = 0;
  cu_bearer->get_tx_sdu_handler().handle_sdu(std::move(sdu));

  // Check DU-> CU-UP path
  byte_buffer             du_buf = make_byte_buffer("DCBA");
  byte_buffer             cu_exp = du_buf.deep_copy();
  byte_buffer_slice_chain du_slice{du_buf.deep_copy()};
  du_bearer->get_tx_sdu_handler().handle_sdu(std::move(du_slice));

  ASSERT_EQ(du_rx.last_sdu, du_exp);
  ASSERT_EQ(cu_rx.last_sdu, cu_exp);
}

int main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}