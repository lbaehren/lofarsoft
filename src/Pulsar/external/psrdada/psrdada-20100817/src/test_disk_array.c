#include "disk_array.h"

#include <stdio.h>

int main ()
{
  disk_array_t* array = 0;
  uint64_t total = 0;
  uint64_t avail = 0;

  fprintf (stderr, "Creating disk array\n");
  array = disk_array_create ();

  fprintf (stderr, "Adding pwd to disk_array\n");
  disk_array_add (array, ".");

  fprintf (stderr, "Adding /tmp to disk_array\n");
  disk_array_add (array, "/tmp");

  fprintf (stderr, "Adding /boot to disk_array\n");
  disk_array_add (array, "/boot");

  total = disk_array_get_total (array);
  avail = disk_array_get_available (array);

  fprintf (stderr, "total: %"PRIu64" bytes   avail: %"PRIu64" bytes\n", total, avail);

  fprintf (stderr, "Closing array\n");
  disk_array_destroy (array);

  return 0;
}
