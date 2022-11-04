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

#include <cstdint>

namespace srsgnb {

/// \brief NZP-CSI-RS-ResourceId is used to identify one NZP-CSI-RS-Resource.
/// \remark See TS 38.331, "NZP-CSI-RS-ResourceId" and "maxNrofNZP-CSI-RS-Resources".
enum nzp_csi_rs_res_id_t : uint8_t {
  MIN_NZP_CSI_RS_RESOURCE_ID   = 0,
  MAX_NZP_CSI_RS_RESOURCE_ID   = 191,
  MAX_NOF_NZP_CSI_RS_RESOURCES = 192
};

} // namespace srsgnb
