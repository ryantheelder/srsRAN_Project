#ifndef SRSGNB_PDCP_PDCP_PACKET_PROCEDURES_H
#define SRSGNB_PDCP_PDCP_PACKET_PROCEDURES_H

#include "srsgnb/ADT/byte_buffer.h"

namespace srsgnb {

/// Collection of packet related procedures of the PDCP interface.
/// This class looks similar to PDCP_input_gateway, however clients are not the same. The first is intended for clients
/// of outer modules that need to pass packets into PDCP, while this interface is intended to process concrete UL data
/// packets, which may be a subcategory of all packets.
class pdcp_packet_procedures
{
public:
  virtual ~pdcp_packet_procedures() = default;

  /// Handles the input UL data packet.
  /// We may have other packet categories which could be handled by adding new methods to this interface.
  virtual void handle_ul_data_packet(byte_buffer& data) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PDCP_PDCP_PACKET_PROCEDURES_H
