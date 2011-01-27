"""Types
"""

from hftools import *

#======================================================================
#  Define Lists of hArray and Vector Types that we will use
#======================================================================

hBaseTypes=[int,float,complex,bool,str]
hBaseNames=["Int","Float","Complex","Bool","String"]
hBaseNamesPy=["int","float","complex","bool","str"]

hRealTypes=[int,float]
hComplexTypes=[complex]
hNumericalTypes=hRealTypes+hComplexTypes
hOtherTypes=[bool,str]

#------------------------------------------------------------

hVectorBaseTypeDictionary={}
hArrayBaseTypeDictionary={}
hArrayTypeDictionary={}
hVectorTypeDictionary={}
hTypeDictionary={}
hTypeNamesDictionary={}
hAllVectorTypes=[]
hAllArrayTypes=[]
for i in range(len(hBaseTypes)):
    btype=hBaseTypes[i]
    vtype=eval(hBaseNames[i]+"Vec")
    atype=eval(hBaseNames[i]+"Array")
    hAllVectorTypes.append(vtype)
    hAllArrayTypes.append(atype)
    hVectorBaseTypeDictionary[vtype]=btype
    hArrayBaseTypeDictionary[atype]=btype
    hVectorTypeDictionary[btype]=vtype
    hArrayTypeDictionary[btype]=atype
    hTypeDictionary[btype]=btype
    hTypeNamesDictionary[btype]=hBaseNamesPy[i]

hAllListTypes=hAllVectorTypes+[list,tuple]
hListAndArrayTypes=[list,tuple]+hAllVectorTypes+hAllArrayTypes

basetypedictionary=dict(zip(hVectorBaseTypeDictionary.keys()+hArrayBaseTypeDictionary.keys()+hTypeDictionary.keys(),hVectorBaseTypeDictionary.values()+hArrayBaseTypeDictionary.values()+hTypeDictionary.values()))

def type2vector(basetype):
    """type2vector(float) -> Vec(0)=[]

    Creates a vector with elements of type 'basetype'. Returns None if
    a vector of the basetype does not exist.
    """
    if basetype in hBaseTypes:
        return hVectorTypeDictionary[basetype]()
    elif basetype == long:
        return hVectorTypeDictionary[int]()
    elif str(basetype)=="<type 'numpy.bool_'>":
        return hVectorTypeDictionary[bool]()
    elif str(basetype)=="<type 'numpy.int32'>":
        return hVectorTypeDictionary[int]()
    elif str(basetype)=="<type 'numpy.int64'>":
        return hVectorTypeDictionary[int]()
    elif str(basetype)=="<type 'numpy.float32'>":
        return hVectorTypeDictionary[float]()
    elif str(basetype)=="<type 'numpy.float64'>":
        return hVectorTypeDictionary[float]()
    elif str(basetype)=="<type 'numpy.complex128'>":
        return hVectorTypeDictionary[complex]()
    else:
        return None

def type2array(basetype):
    """type2array(float) -> Vec(0)=[]

    Creates an array with elements of type 'basetype'. Returns None if
    an array of the basetype does not exist.
    """
    if basetype in hBaseTypes:
        return hArrayTypeDictionary[basetype]()
    else: return None

def basetype(vec_or_array):
    """
    basetype(IntVec) -> <type 'int'>
    basetype(FloatArray) -> <type 'float'>
    basetype(complex) -> <type 'complex'>

    Returns the basic type contained in the vector or array. Input can be either a vector, an array, or a type thereof.
    """
    t=type(vec_or_array)
    if t==type: t=vec_or_array
    return basetypedictionary[t]

def typename(btype):
    """
    basetype(float) -> "float"
    basetype(str) -> "str"

    Returns the name of the base type as a short string.
    """
    t=type(btype)
    if t==type: t=btype
    return hTypeNamesDictionary[t]

hRealVectorTypes=map(lambda t:hVectorTypeDictionary[t],hRealTypes)
hComplexVectorTypes=map(lambda t:hVectorTypeDictionary[t],hComplexTypes)
hNumericalVectorTypes=map(lambda t:hVectorTypeDictionary[t],hNumericalTypes)
hOtherVectorTypes=map(lambda t:hVectorTypeDictionary[t],hOtherTypes)

hRealArrayTypes=map(lambda t:hArrayTypeDictionary[t],hRealTypes)
hComplexArrayTypes=map(lambda t:hArrayTypeDictionary[t],hComplexTypes)
hNumericalArrayTypes=map(lambda t:hArrayTypeDictionary[t],hNumericalTypes)
hOtherArrayTypes=map(lambda t:hArrayTypeDictionary[t],hOtherTypes)

hRealContainerTypes=hRealVectorTypes+hRealArrayTypes
hComplexContainerTypes=hComplexVectorTypes+hComplexArrayTypes
hNumericalContainerTypes=hNumericalVectorTypes+hNumericalArrayTypes
hOtherContainerTypes=hOtherVectorTypes+hOtherArrayTypes
hAllContainerTypes=hAllVectorTypes+hAllArrayTypes

"""
The following variables define list of methods that are assigned to
vectors and hArrays depending on the types that are
available. E.g. hNew is available for hArrays of all types, while
invFFT is only available for comlex vector and array types.

Whenever a new function is defined on the c-side, in the hftools library, it should be added here as well.
"""
hAllVectorMethods = ["hResize","hNew"]

hAllContainerMethods = ["hFill","hSet","hFind","hFlip","hCopy","hSort","hZipper","hReadDump","hWriteDump","hRedistribute","hPPrint","hPrettyString","hReadTextTable"]

hRealContainerMethods = ["hMean","hMeanAbs","hMeanSquare","hStdDev","hStdDevBelow","hMeanThreshold","hMeanInverse","hDownsample","hUpsample","hDownsampleSpikyData","hInterpolate2P","hInterpolate2PSubpiece","hNegate","hVectorLength","hNormalize","hAcos","hAsin","hAtan","hCeil","hFloor","hMeanGreaterThanThreshold","hMeanGreaterEqualThreshold","hMeanLessThanThreshold","hMeanLessEqualThreshold","hRunningAverage","hDelayToPhase","hInvFFTCasa","hFFTw","hInvFFTw","hSaveInvFFTw","hGetHanningFilter","hApplyHanningFilter","hGetHanningFilterHalf","hSpectralPower","hRFIDownsampling","hRFIBaselineFitting","hRFIFlagging","hLinearFitPolynomialX","hLinearFit","hBSplineFitXValues","hBSpline","hBSplineFit","hErrorsToWeights","hPolynomial"]

hComplexContainerMethods = ["hConj","hCrossCorrelateComplex","hFFTCasa","hInvFFTw","hSaveInvFFTw","hFFTw","hNyquistSwap","hPhaseToComplex","hAmplitudePhaseToComplex","hRFIDownsampling","hSetAmplitude","hDoubleFFT","hDoubleFFT1","hDoubleFFT2","hDoubleFFTPhaseMul","hDoubleFFTPhase"]

hNumericalContainerMethods = ["hFillRange","hAbs","hMax","hMaxPos","hMin","hMinPos","hConvert","hConvertResize","hMul","hDiv","hSub","hAdd","hMulTo","hDivTo","hSubTo","hAddTo","hMulAdd","hDivAdd","hSubAdd","hAddAdd","hArg","hImag","hNorm","hReal","hCos","hCosh","hExp","hLog","hLog10","hLogSave","hSqrtAbs","hSin","hSinh","hSqrt","hSquare","hTan","hTanh","hSum","hSumAbs","hSumSquare","hMulSum","hDiffSum","hDiffSquaredSum","hMeanDiffSquaredSum","hChiSquared","hMeanChiSquared","hRandom","hSortMedian","hMedian","hFindLowerBound","hCrossProduct","hFmod","hMulAdd2","hDivAdd2","hSubAdd2","hAddAdd2","hMulSelf","hDivSelf","hSubSelf","hAddSelf","hFindGreaterThan","hFindGreaterEqual","hFindLessThan","hFindLessEqual","hFindGreaterThanAbs","hFindGreaterEqualAbs","hFindLessThanAbs","hFindLessEqualAbs","hCountGreaterThan","hCountGreaterEqual","hCountGreaterThanAbs","hCountGreaterEqualAbs","hCountLessThan","hCountLessEqual","hCountLessThanAbs","hCountLessEqualAbs","hFindBetween","hFindBetweenOrEqual","hFindOutside","hFindOutsideOrEqual","hFindSequenceGreaterThan","hFindSequenceGreaterEqual","hFindSequenceLessThan","hFindSequenceLessEqual","hFindSequenceBetween","hFindSequenceBetweenOrEqual","hFindSequenceOutside","hFindSequenceOutsideOrEqual"]

