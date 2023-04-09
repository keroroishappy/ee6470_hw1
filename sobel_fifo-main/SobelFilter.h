#ifndef SOBEL_FILTER_H_
#define SOBEL_FILTER_H_
#include <systemc>
using namespace sc_core;

#include "filter_def.h"

class Filter : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;
  sc_fifo_in<unsigned int> i_r;
  sc_fifo_in<unsigned int> i_g;
  sc_fifo_in<unsigned int> i_b;
  sc_fifo_out<int> o_result;
  sc_event event1;
  SC_HAS_PROCESS(Filter);
  Filter(sc_module_name n);
  ~Filter() = default;

private:
  void do_filter();
  void do_output();
  // void do_mean();
  float val;
  float val2;
};
#endif
