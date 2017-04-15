/* -*- c++ -*- */
/*
 * Copyright 2015 Pavel Demin
 * Copyright 2015 Dimitri Stolnikov <horiz0n@gmx.net>
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#ifndef REDPITAYA_SINK_C_H
#define REDPITAYA_SINK_C_H

#include <gnuradio/sync_block.h>

#include "sink_iface.h"

#include "redpitaya_common.h"

class redpitaya_sink_c;

typedef boost::shared_ptr< redpitaya_sink_c > redpitaya_sink_c_sptr;

redpitaya_sink_c_sptr make_redpitaya_sink_c( const std::string & args = "" );

class redpitaya_sink_c :
    public gr::sync_block,
    public sink_iface
{
private:
  friend redpitaya_sink_c_sptr make_redpitaya_sink_c(const std::string &args);

  redpitaya_sink_c(const std::string &args);

public:
  ~redpitaya_sink_c();

  int work( int noutput_items,
            gr_vector_const_void_star &input_items,
            gr_vector_void_star &output_items );

  std::string name();

  static std::vector< std::string > get_devices( bool fake = false );

  size_t get_num_channels( void );

  osmosdr::meta_range_t get_sample_rates( void );
  double set_sample_rate( double rate );
  double get_sample_rate( void );

  osmosdr::freq_range_t get_freq_range( size_t chan = 0 );
  double set_center_freq( double freq, size_t chan = 0 );
  double get_center_freq( size_t chan = 0 );
  double set_freq_corr( double ppm, size_t chan = 0 );
  double get_freq_corr( size_t chan = 0 );

  std::vector<std::string> get_gain_names( size_t chan = 0 );
  osmosdr::gain_range_t get_gain_range( size_t chan = 0 );
  osmosdr::gain_range_t get_gain_range( const std::string & name, size_t chan = 0 );
  double set_gain( double gain, size_t chan = 0 );
  double set_gain( double gain, const std::string & name, size_t chan = 0 );
  double get_gain( size_t chan = 0 );
  double get_gain( const std::string & name, size_t chan = 0 );

  std::vector< std::string > get_antennas( size_t chan = 0 );
  std::string set_antenna( const std::string & antenna, size_t chan = 0 );
  std::string get_antenna( size_t chan = 0 );

private:
  double _freq, _rate, _corr;
  SOCKET _sockets[2];
};

#endif // REDPITAYA_SINK_C_H
