/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/optional.h"
#include "srsran/asn1/e2ap/e2sm.h"
#include "srsran/asn1/e2ap/e2sm_kpm.h"

namespace srsran {

/* Metric Levels */
enum e2sm_kpm_metric_level_enum {
  E2_NODE_LEVEL  = 0x01,
  UE_LEVEL       = 0x02,
  QOS_FLOW_LEVEL = 0x04,
  ALL_LEVELS     = 0x07,
  UNKNOWN_LEVEL  = 0x10
};

/* Metric Labels */
enum e2sm_kpm_label_enum {
  NO_LABEL                  = 0x00000001, // if included, other labels shall not be included (and vice versa)
  PLMN_ID_LABEL             = 0x00000002,
  SLICE_ID_LABEL            = 0x00000004,
  FIVE_QI_LABEL             = 0x00000008,
  QFI_LABEL                 = 0x00000010,
  QCI_LABEL                 = 0x00000020,
  QCI_MAX_LABEL             = 0x00000040,
  QCI_MIN_LABEL             = 0x00000080,
  ARP_MAX_LABEL             = 0x00000100,
  ARP_MIN_LABEL             = 0x00000200,
  BITRATE_RANGE_LABEL       = 0x00000400,
  LAYER_MU_MIMO_LABEL       = 0x00000800,
  SUM_LABEL                 = 0x00001000,
  DIST_BIN_X_LABEL          = 0x00002000,
  DIST_BIN_Y_LABEL          = 0x00004000,
  DIST_BIN_Z_LABEL          = 0x00008000,
  PRE_LABEL_OVERRIDE_LABEL  = 0x00010000,
  START_END_IND_LABEL       = 0x00020000,
  MIN_LABEL                 = 0x00040000,
  MAX_LABEL                 = 0x00080000,
  AVG_LABEL                 = 0x00100000,
  SSB_IDX_LABEL             = 0x00200000,
  NON_GO_B_BFMODE_IDX_LABEL = 0x00400000,
  MIMO_MODE_IDX_LABEL       = 0x00800000,
  ALL_VALUE_TYPE_LABELS     = 0x001c1000, // SUM | MIN | MAX | AVG
  ALL_SUBCOUNTER_LABELS     = 0x00e3effe,
  ALL_LABELS                = 0x00ffffff,
  UNKNOWN_LABEL             = 0x10000000
};

class e2sm_kpm_meas_provider
{
public:
  virtual ~e2sm_kpm_meas_provider() = default;

  /// \brief check if cell with cell global id is supported
  /// \param[in] cell_global_id of the required cell
  /// \return Returns True if cell is present
  virtual bool cell_supported(const asn1::e2sm_kpm::cgi_c& cell_global_id) = 0;

  /// \brief check if UE with ueid is supported
  /// \param[in] ueid of the required UE
  /// \return Returns True if UE is present
  virtual bool ue_supported(const asn1::e2sm_kpm::ueid_c& ueid) = 0;

  /// \brief check if a test condition is supported
  /// \param[in] test_cond_type
  /// \return Returns True if test condition is supported
  virtual bool test_cond_supported(const asn1::e2sm_kpm::test_cond_type_c& test_cond_type) = 0;

  /// \brief check if a metric with labels, level and optional cell scope is supported
  /// \param[in] meas_type defines the metric
  /// \param[in] label if metric can be measured with the given label
  /// \param[in] level if metric can be measured at the given level
  /// \param[in] cell_scope if metric can be measured for a single cell
  /// \return Returns True if metric is supported with given labels, level and cell_scope
  virtual bool metric_supported(const asn1::e2sm_kpm::meas_type_c&  meas_type,
                                const asn1::e2sm_kpm::meas_label_s& label,
                                const e2sm_kpm_metric_level_enum    level,
                                const bool&                         cell_scope) = 0;

  /// \brief collected measurements for the given metric
  /// \param[in] meas_type defines the metric to measure
  /// \param[in] label_info_list defines labels for the meas_type
  /// \param[in] ues if not empty, then measure meas_info_item for the listed UE IDs
  /// \param[in] cell_global_id if present measure meas_info_item within the cell scope
  /// \param[out] items contains measruement records (if ues is not empty, then one record for each UE)
  /// \return Returns True if measurement collection was successful
  virtual bool get_meas_data(const asn1::e2sm_kpm::meas_type_c&               meas_type,
                             const asn1::e2sm_kpm::label_info_list_l          label_info_list,
                             const std::vector<asn1::e2sm_kpm::ueid_c>&       ues,
                             const optional<asn1::e2sm_kpm::cgi_c>            cell_global_id,
                             std::vector<asn1::e2sm_kpm::meas_record_item_c>& items) = 0;
};

} // namespace srsran