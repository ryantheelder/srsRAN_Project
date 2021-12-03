#include "srsgnb/pdcp/pdcp_entity_factory.h"
#include "pdcp_entity_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsgnb;

/// Factories are at a low level point of abstraction, as such, they are the "only" (best effort) objects that depend on
/// concrete class implementations instead of interfaces, intrinsically giving them tight coupling to the objects being
/// created. Keeping this coupling in a single file, is the best, as the rest of the code can be kept decoupled.

std::unique_ptr<pdcp_entity> srsgnb::create_pdcp_entity()
{
  return std::unique_ptr<pdcp_entity>(new pdcp_entity_impl);
}
