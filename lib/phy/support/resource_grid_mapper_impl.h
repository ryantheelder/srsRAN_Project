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
#include "srsran/phy/generic_functions/precoding/channel_precoder.h"
#include "srsran/phy/support/re_buffer.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/phy/support/resource_grid_writer.h"

namespace srsran {

/// Implements a resource grid mapper.
class resource_grid_mapper_impl : public resource_grid_mapper
{
public:
  resource_grid_mapper_impl(unsigned                          nof_ports_,
                            unsigned                          nof_symb_,
                            unsigned                          nof_subc_,
                            resource_grid_writer&             writer_,
                            std::unique_ptr<channel_precoder> precoder_);

  // See interface for documentation.
  void
  map(const re_buffer_reader& input, const re_pattern_list& pattern, const precoding_configuration& precoding) override;

  // See interface for documentation.
  void map(const re_buffer_reader&        input,
           const re_pattern_list&         pattern,
           const re_pattern_list&         reserved,
           const precoding_configuration& precoding) override;

  // See interface for documentation.
  void map(symbol_buffer&                 buffer,
           const re_pattern_list&         pattern,
           const re_pattern_list&         reserved,
           const precoding_configuration& precoding) override;

private:
  /// Maximum number of symbols that can be layer mapped, precoded and mapped onto the resource grid at once.
  static constexpr unsigned MAX_NOF_SYMBOLS = 512;

  /// Resource grid dimensions.
  unsigned nof_ports;
  unsigned nof_symb;
  unsigned nof_subc;

  /// Resource grid writer.
  resource_grid_writer& writer;

  /// Channel precoder.
  std::unique_ptr<channel_precoder> precoder;

  /// Temporal layer mapping output buffer, used to store data between layer mapping and precoding.
  dynamic_re_buffer layer_mapping_buffer;

  /// Temporal output buffer, used to store the Resource Elements after precoding.
  dynamic_re_buffer precoding_buffer;
};

} // namespace srsran
