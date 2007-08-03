
#-------------------------------------------------------------------------------
# $Id: Masking.g,v 1.1 2006/03/06 15:34:52 horneff Exp $
#-------------------------------------------------------------------------------
#
# Operations for logical/masking arrays
#
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#Name: mergeMasks
#-------------------------------------------------------------------------------
#Type: Glish function
#Doc:  Merge two logical/masking array.
#Created: 2005/02/14 (Lars B&auml;hren)
#
#Par:  mask1          - First masking array.
#Par:  mask2          - Second masking array.
#Par:  logic = 'AND'  - Boolean logic by which to combine the masks.
#                       (AND, NAND, OR, NOR)
#
#Ret:  mask           - Boolean array with the masks merged according to the
#                       'logic' parameter.
#
#Example: a := array(T,5,5); b := array(F,5,5); mergeMasks (a,b)
#-------------------------------------------------------------------------------

mergeMasks := function (ref mask1, ref mask2, logic='AND')
{

  # Check if the shapes of the array are consistent

  shape1 := shape(mask1);
  shape2 := shape(mask2);

  if (shape1 != shape2) {
    print '[Masking::mergeMasks] Inconsistent array shapes!';
    print ' Array 1 ',shape1;
    print ' Array 2 ',shape2;
  } 
  else {
    if (logic == 'AND') {
      mask := as_boolean(mask1*mask2);
    }
    else if (logic == 'NAND') {
      mask := !as_boolean(mask1*mask2);
    }
    else if (logic == 'OR') {
      mask := as_boolean(mask1+mask2);
    }
    else if (logic == 'NOR') {
      mask := !as_boolean(mask1+mask2);
    }
    else {
      print '[Masking::mergeMasks] Unsupported merging option',logic;
      mask := F;
    }
  }

  return mask;
}
