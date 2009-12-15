#/^ *from xxx import/ {next}
#/ToolTipText/ {next}
#/ToolTipBase/ {next}
/^ *from hfglobal/ {print; next}
/hfQtPlotWidget/ {sub("hfQtPlotWidget","hfglobal.hfQtPlotWidget")}
{print}