// MSTable_1000.cc -- Mon Nov 13 15:01:16 CET 2006 -- root
#include <ms/MeasurementSets/MSTable.cc>
#include <ms/MeasurementSets/MeasurementSet.h>
namespace casa { //# NAMESPACE - BEGIN
template class MSTable<MSAntennaEnums::PredefinedColumns, MSAntennaEnums::PredefinedKeywords>;
template class MSTable<MSDataDescriptionEnums::PredefinedColumns, MSDataDescriptionEnums::PredefinedKeywords>;
template class MSTable<MSDopplerEnums::PredefinedColumns, MSDopplerEnums::PredefinedKeywords>;
template class MSTable<MSFeedEnums::PredefinedColumns, MSFeedEnums::PredefinedKeywords>;
template class MSTable<MSFieldEnums::PredefinedColumns, MSFieldEnums::PredefinedKeywords>;
template class MSTable<MSFlagCmdEnums::PredefinedColumns, MSFlagCmdEnums::PredefinedKeywords>;
template class MSTable<MSFreqOffsetEnums::PredefinedColumns, MSFreqOffsetEnums::PredefinedKeywords>;
template class MSTable<MSHistoryEnums::PredefinedColumns, MSHistoryEnums::PredefinedKeywords>;
template class MSTable<MSMainEnums::PredefinedColumns, MSMainEnums::PredefinedKeywords>;
template class MSTable<MSObservationEnums::PredefinedColumns, MSObservationEnums::PredefinedKeywords>;
template class MSTable<MSPointingEnums::PredefinedColumns, MSPointingEnums::PredefinedKeywords>;
template class MSTable<MSPolarizationEnums::PredefinedColumns, MSPolarizationEnums::PredefinedKeywords>;
template class MSTable<MSProcessorEnums::PredefinedColumns, MSProcessorEnums::PredefinedKeywords>;
template class MSTable<MSSourceEnums::PredefinedColumns, MSSourceEnums::PredefinedKeywords>;
template class MSTable<MSSpectralWindowEnums::PredefinedColumns, MSSpectralWindowEnums::PredefinedKeywords>;
template class MSTable<MSStateEnums::PredefinedColumns, MSStateEnums::PredefinedKeywords>;
template class MSTable<MSSysCalEnums::PredefinedColumns, MSSysCalEnums::PredefinedKeywords>;
template class MSTable<MSWeatherEnums::PredefinedColumns, MSWeatherEnums::PredefinedKeywords>;
} //# NAMESPACE - END
