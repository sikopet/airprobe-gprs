//
// C++ Implementation: gsm_receiver_config
//
// Description:
// This file contains classes which define gsm_receiver configuration
// and the burst_counter which is used to store internal state of the receiver
// when it's synchronized
//
//
// Author: Piotr Krysik <perper@o2.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gsm_receiver_config.h>

burst_counter & burst_counter::operator++(int)
{
  d_timeslot_nr++;
  if (d_timeslot_nr == TS_PER_FRAME) {
    d_timeslot_nr = 0;

    if ((d_t2 == 25) && (d_t3 == 50)) {
      d_t1 = (d_t1 + 1) % (1 << 11);
    }

    d_t2 = (d_t2 + 1) % 26;
    d_t3 = (d_t3 + 1) % 51;
  }

  //update offset - this is integer for d_OSR which is multiple of four
  d_offset_fractional += GUARD_FRACTIONAL * d_OSR;
  d_offset_integer = floor(d_offset_fractional);
  d_offset_fractional = d_offset_fractional - d_offset_integer;
  return (*this);
}

void burst_counter::set(uint32_t t1, uint32_t t2, uint32_t t3, uint32_t timeslot_nr)
{
  d_t1 = t1;
  d_t2 = t2;
  d_t3 = t3;
  d_timeslot_nr = timeslot_nr;
  double first_sample_position = (get_frame_nr() * 8 + timeslot_nr) * TS_BITS;
  d_offset_fractional = first_sample_position - floor(first_sample_position);
  d_offset_integer = 0;
}

burst_type channel_configuration::get_burst_type(burst_counter burst_nr)
{
  uint32_t timeslot_nr = burst_nr.get_timeslot_nr();
  multiframe_type m_type = d_timeslots_descriptions[timeslot_nr].get_type();
  uint32_t nr;

  switch (m_type) {
    case multiframe_26:
      nr = burst_nr.get_t2();
      break;
    case multiframe_51:
      nr = burst_nr.get_t3();
      break;
    default:
      nr = 0;
      break;
  }

  return d_timeslots_descriptions[timeslot_nr].get_burst_type(nr);
}
