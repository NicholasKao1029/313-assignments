#include <limits.h>

long maxsa(long *array, long size, long **sa_start, long **sa_end) {

  if (size == 1) {
    *sa_start = *sa_end = array;
    return *array;
  }

  long half = size >> 1;
  long *mid = array + half, *end = array + size - 1;

  long *ptr;
  long sum, leftsum = LONG_MIN, rightsum = LONG_MIN;

  //from the mid point 
  // as long as the ptr is in the array bounds dec
  // then each time add the value of which the ptr points to sum
  // if the addition is benefitial 
  // then leftsum is now sum 
  // sa start is points to ptr
  for (ptr = mid - 1, sum = 0; ptr >= array; ptr--) {
    sum += *ptr;
    if (sum > leftsum) {
      leftsum = sum;
      *sa_start = ptr;
    }
  }
  

  //from mid point 
  // go to the end 
  // add to sum 
  // if sum is better than right sum 
  // overwrite right sum 
  // and put the new pointer 
  for (ptr = mid, sum = 0; ptr <= end; ptr++) {
    sum += *ptr;
    if (sum > rightsum) {
      rightsum = sum;
      *sa_end = ptr;
    }
  }

  // best sum will be the total of both sums
  // 
  long bestsum = leftsum + rightsum;
  long *sub_start, *sub_end;

  //recurse on the beggining array 
  
  sum = maxsa(array, half, &sub_start, &sub_end);
  if (sum > bestsum) {
    bestsum = sum;
    *sa_start = sub_start;
    *sa_end = sub_end;
  }

  //recurse on the second half ot the current array
  
  sum = maxsa(mid, size - half, &sub_start, &sub_end);
  if (sum > bestsum) {
    bestsum = sum;
    *sa_start = sub_start;
    *sa_end = sub_end;
  }

  return bestsum;
}
