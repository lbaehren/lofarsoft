#include "FTransform.h"
#include <assert.h>

using namespace FTransform;

int main () {

  // test the positive
  assert (frc & real);
  assert (frc & forward);

  assert (bcr & real);
  assert (bcr & backward);

  assert (fcc & analytic);
  assert (fcc & forward);

  assert (bcc & analytic);
  assert (bcc & backward);

  // test the negative
  assert (!(frc & analytic));
  assert (!(frc & backward));

  assert (!(bcr & analytic));
  assert (!(bcr & forward));

  assert (!(fcc & real));
  assert (!(fcc & backward));

  assert (!(bcc & real));
  assert (!(bcc & forward));

  return 0;
}

