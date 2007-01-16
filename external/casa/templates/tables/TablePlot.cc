
#include <tables/TablePlot/BasePlot.cc>
#include <tables/TablePlot/CrossPlot.cc>
#include <tables/TablePlot/TPPlotter.cc>
#include <tables/TablePlot/TablePlot.cc>

namespace casa {

  template class BasePlot<Float>;
  template class CrossPlot<Float>;
  template class TPPlotter<Float>;
  template class TablePlot<Float>;

}
