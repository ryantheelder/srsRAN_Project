#ifndef SRSGNB_SDAP_SDAP_ENTITY_H
#define SRSGNB_SDAP_SDAP_ENTITY_H

#include "srsgnb/ADT/byte_buffer.h"

namespace srsgnb {

/// SDAP entity receiving side. Retrieves an SDAP SDU from a lower layer data PDU.
class sdap_entity
{
public:
  virtual ~sdap_entity() = default;

  /// Decapsulates the SDAP header in the provided PDU converting it into an SDAP SDU.
  void decapsulate(byte_buffer& data);
};

} // namespace srsgnb

#endif // SRSGNB_SDAP_SDAP_ENTITY_H
