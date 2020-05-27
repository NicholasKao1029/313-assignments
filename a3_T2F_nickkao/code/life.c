#include <stdio.h>
#include "life.h"

// You are only allowed to change the contents of this file with respect 
// to improving the performance of this program



// If you need to initialize some data structures for your implementation
// of life then make changes to the following function, otherwise 
// do not change this function.

void initLife() {
}

//Log 11
#define USE
#ifdef USE
int life(long oldWorld[N][N], long newWorld[N][N]) {

  int i, j, k, l;
  int col, row, col1, row1;
  int q = -1;
  int alive = 0;
  // Count the cells to the top left
  for (j = 0; j < N; j++) {
    row = (j - 1 + N ) % N;
    row1 = (j + 1 + N ) % N;
    
    for (i = 0; i < N; i++) {
      col = (i -1 + N) % N;
      col1 = (i + 1 + N) % N;


      k = 0;
      k += oldWorld[row][col];
      k += oldWorld[j][col];
      k += oldWorld[row][i];
      k += oldWorld[row][col1];
      k += oldWorld[j][col1];
      k += oldWorld[row1][col];
      k += oldWorld[row1][col1];
      k += oldWorld[row1][i];
      newWorld[j][i] = checkHealth(k, oldWorld[j][i]);
      alive += newWorld[j][i] ? 1:0;
    }
  }
  // Check each cell to see if it should come to life, continue to live, or die
  
  return alive;
}
#undef USE
#endif

//Log 10
#ifdef USE
int life(long oldWorld[N][N], long newWorld[N][N]) {

  int i, j, k, l;
  int col, row, col1, row1;
  int q = -1;
  int alive = 0;
  // Count the cells to the top left
  for (i = 0; i < N; i++) {
    col = (i -1 + N) % N;
    col1 = (i + 1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j - 1 + N ) % N;
      row1 = (j + 1 + N ) % N;


      k = 0;
      k += oldWorld[row][col];
      k += oldWorld[j][col];
      k += oldWorld[row][i];
      k += oldWorld[row][col1];
      k += oldWorld[j][col1];
      k += oldWorld[row1][col];
      k += oldWorld[row1][col1];
      k += oldWorld[row1][i];
      newWorld[j][i] = checkHealth(k, oldWorld[j][i]);
      alive += newWorld[j][i] ? 1:0;
    }
  }
  // Check each cell to see if it should come to life, continue to live, or die
  
  return alive;
}
#undef USE
#endif


//Log 9

#ifdef USE
int life(long oldWorld[N][N], long newWorld[N][N]) {

  int i, j, k, l;
  int col, row, col1, row1;
  int q = -1;
  int alive = 0;

  // Count the cells to the top left
  for (i = 0; i < N; i++) {
    col = (i -1 + N) % N;
    col1 = (i + 1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j - 1 + N ) % N;
      row1 = (j + 1 + N ) % N;
      k = 0;
      k += oldWorld[row][col];
      k += oldWorld[j][col];
      k += oldWorld[row][i];
      k += oldWorld[row][col1];
      k += oldWorld[j][col1];
      k += oldWorld[row1][col];
      k += oldWorld[row1][col1];
      k += oldWorld[row1][i];
      newWorld[j][i] = k;
      newWorld[j][i] = checkHealth(newWorld[j][i], oldWorld[j][i]);
      alive += newWorld[j][i] ? 1:0;
    }
  }
  // Check each cell to see if it should come to life, continue to live, or die
  
  return alive;
}
#undef USE
#endif

//Log 8
#ifdef USE
int life(long oldWorld[N][N], long newWorld[N][N]) {

  int i, j, k, l;
  
  //clear the new world
  // for (i = 0; i < N; i++)
  //   for (j = 0; j < N; j++) {
     
  //   }

  int col, row, col1, row1;
  int q = -1;
  int alive = 0;

  // Count the cells to the top left
  for (i = 0; i < N; i++) {
    col = (i -1 + N) % N;
    col1 = (i + 1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j - 1 + N ) % N;
      row1 = (j + 1 + N ) % N;
      newWorld[j][i] =  0;
      newWorld[j][i] += oldWorld[row][col];
      newWorld[j][i] += oldWorld[j][col];
      newWorld[j][i] += oldWorld[row][i];
      newWorld[j][i] += oldWorld[row][col1];
      newWorld[j][i] += oldWorld[j][col1];
      newWorld[j][i] += oldWorld[row1][col];
      newWorld[j][i] += oldWorld[row1][col1];
      newWorld[j][i] += oldWorld[row1][i];
      newWorld[j][i] = checkHealth(newWorld[j][i], oldWorld[j][i]);
      alive += newWorld[j][i] ? 1:0;
    }
  }
  // Check each cell to see if it should come to life, continue to live, or die
  
  return alive;
}
#undef USE
#endif

//Log 7
#ifdef USE
int life(long oldWorld[N][N], long newWorld[N][N]) {

  int i, j, k, l;
  
  //clear the new world
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++) {
      newWorld[j][i] =  0;
    }

  int col, row, col1, row1;
  int q = -1;


  // Count the cells to the top left
  for (i = 0; i < N; i++) {
    col = (i -1 + N) % N;
    col1 = (i + 1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j - 1 + N ) % N;
      row1 = (j + 1 + N ) % N;
      newWorld[j][i] += oldWorld[row][col];
      newWorld[j][i] += oldWorld[j][col];
      newWorld[j][i] += oldWorld[row][i];

      newWorld[j][i] += oldWorld[row][col1];
      newWorld[j][i] += oldWorld[j][col1];

      newWorld[j][i] += oldWorld[row1][col];
      newWorld[j][i] += oldWorld[row1][col1];
      newWorld[j][i] += oldWorld[row1][i];
    }
  }


  // Count the cells to the top right
//   for (i = 0; i < N; i++) {
//     col1 = (i + 1 + N) % N;
//     for (j = 0; j < N; j++) {
//       row = (j - 1  + N ) % N;
      
//     }
//   }


// // Count the cells to the bottom left
//   for (i = 0; i < N; i++) {
//     col = (i - 1 + N) % N;
    
//     for (j = 0; j < N; j++) {
//       row1 = (j + 1 + N ) % N;
      
//     }
//   }


  // Check each cell to see if it should come to life, continue to live, or die
  int alive = 0;
  
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++) {
      newWorld[j][i] = checkHealth(newWorld[j][i], oldWorld[j][i]);
      alive += newWorld[j][i] ? 1:0;
    }

  return alive;
}
#undef USE
#endif

//Log 6
#ifdef USE
int life(long oldWorld[N][N], long newWorld[N][N]) {

  int i, j, k, l;
  
  //clear the new world
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++) {
      newWorld[j][i] =  0;
    }

  int col, row, col1;
  int q = -1;


  // Count the cells to the top left
  for (i = 0; i < N; i++) {
    col = (i -1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j - 1 + N ) % N;
      newWorld[j][i] += oldWorld[row][col];
      newWorld[j][i] += oldWorld[j][col];
      newWorld[j][i] += oldWorld[row][i];
    }
  }
  
  // Count the cells immediately above
  // for (i = 0; i < N; i++) {
  //   for (j = 0; j < N; j++) {
  //     row = (j - 1 + N ) % N;
      
  //   }
  // }

  // Count the cells to the top right
  for (i = 0; i < N; i++) {
    col = (i + 1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j - 1  + N ) % N;
      newWorld[j][i] += oldWorld[row][col];
      newWorld[j][i] += oldWorld[j][col];
    }
  }

 // Count the cells to the immediate left
  // for (i = 0; i < N; i++) {
  //   col = (i -1 + N) % N;
  //   for (j = 0; j < N; j++) {
       
  //   }
  // }
  
  // Count the cells to the immediate right
  // for (i = 0; i < N; i++) {
  //   col = (i + 1 + N) % N;
  //   for (j = 0; j < N; j++) {
      
  //   }
  // }

// Count the cells to the bottom left
  for (i = 0; i < N; i++) {
    col = (i - 1 + N) % N;
    col1 = (i + 1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j + 1 + N ) % N;
      newWorld[j][i] += oldWorld[row][col];
      newWorld[j][i] += oldWorld[row][col1];
      newWorld[j][i] += oldWorld[row][i];
    }
  }

  // Count the cells immediately below
  // for (i = 0; i < N; i++) {
  //   for (j = 0; j < N; j++) {
  //     row = (j + 1  + N ) % N;
      
  //   }
  // }

  // Count the cells to the bottom right
  // for (i = 0; i < N; i++) {
  //   col = (i + 1 + N) % N;
  //   for (j = 0; j < N; j++) {
  //     row = (j + 1  + N ) % N;
      
  //   }
  // }

  // Check each cell to see if it should come to life, continue to live, or die
  int alive = 0;
  
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++) {
      newWorld[j][i] = checkHealth(newWorld[j][i], oldWorld[j][i]);
      alive += newWorld[j][i] ? 1:0;
    }

  return alive;
}
#undef USE
#endif

//Log 5
#ifdef USE
int life(long oldWorld[N][N], long newWorld[N][N]) {

  int i, j, k, l;
  
  //clear the new world
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++) {
      newWorld[j][i] =  0;
    }

  int col, row;
  int q = -1;


  // Count the cells to the top left
  for (i = 0; i < N; i++) {
    col = (i -1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j - 1 + N ) % N;
      newWorld[j][i] += oldWorld[row][col];
      newWorld[j][i] += oldWorld[j][col];
      newWorld[j][i] += oldWorld[row][i];
    }
  }
  
  // Count the cells immediately above
  // for (i = 0; i < N; i++) {
  //   for (j = 0; j < N; j++) {
  //     row = (j - 1 + N ) % N;
      
  //   }
  // }

  // Count the cells to the top right
  for (i = 0; i < N; i++) {
    col = (i + 1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j - 1  + N ) % N;
      newWorld[j][i] += oldWorld[row][col];
      newWorld[j][i] += oldWorld[j][col];
    }
  }

 // Count the cells to the immediate left
  // for (i = 0; i < N; i++) {
  //   col = (i -1 + N) % N;
  //   for (j = 0; j < N; j++) {
       
  //   }
  // }
  
  // Count the cells to the immediate right
  // for (i = 0; i < N; i++) {
  //   col = (i + 1 + N) % N;
  //   for (j = 0; j < N; j++) {
      
  //   }
  // }

// Count the cells to the bottom left
  for (i = 0; i < N; i++) {
    col = (i - 1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j + 1 + N ) % N;
      newWorld[j][i] += oldWorld[row][col];
      newWorld[j][i] += oldWorld[row][i];
    }
  }

  // Count the cells immediately below
  // for (i = 0; i < N; i++) {
  //   for (j = 0; j < N; j++) {
  //     row = (j + 1  + N ) % N;
      
  //   }
  // }

  // Count the cells to the bottom right
  for (i = 0; i < N; i++) {
    col = (i + 1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j + 1  + N ) % N;
      newWorld[j][i] += oldWorld[row][col];
    }
  }

  // Check each cell to see if it should come to life, continue to live, or die
  int alive = 0;
  
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++) {
      newWorld[j][i] = checkHealth(newWorld[j][i], oldWorld[j][i]);
      alive += newWorld[j][i] ? 1:0;
    }

  return alive;
}
#undef USE
#endif

//Log 4
#ifdef USE
int life(long oldWorld[N][N], long newWorld[N][N]) {

  int i, j, k, l;
  
  //clear the new world
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++) {
      newWorld[j][i] =  0;
    }

  int col, row;
  int q = -1;


  // Count the cells to the top left
  for (i = 0; i < N; i++) {
    col = (i -1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j - 1 + N ) % N;
      newWorld[j][i] += oldWorld[row][col];
      newWorld[j][i] += oldWorld[j][col];
      newWorld[j][i] += oldWorld[row][i];
    }
  }
  
  // Count the cells immediately above
  // for (i = 0; i < N; i++) {
  //   for (j = 0; j < N; j++) {
  //     row = (j - 1 + N ) % N;
      
  //   }
  // }

  // Count the cells to the top right
  for (i = 0; i < N; i++) {
    col = (i + 1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j - 1  + N ) % N;
      newWorld[j][i] += oldWorld[row][col];
      newWorld[j][i] += oldWorld[j][col];
    }
  }

 // Count the cells to the immediate left
  // for (i = 0; i < N; i++) {
  //   col = (i -1 + N) % N;
  //   for (j = 0; j < N; j++) {
       
  //   }
  // }
  
  // Count the cells to the immediate right
  // for (i = 0; i < N; i++) {
  //   col = (i + 1 + N) % N;
  //   for (j = 0; j < N; j++) {
      
  //   }
  // }

// Count the cells to the bottom left
  for (i = 0; i < N; i++) {
    col = (i - 1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j + 1 + N ) % N;
      newWorld[j][i] += oldWorld[row][col];
    }
  }

  // Count the cells immediately below
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      row = (j + 1  + N ) % N;
      newWorld[j][i] += oldWorld[row][i];
    }
  }

  // Count the cells to the bottom right
  for (i = 0; i < N; i++) {
    col = (i + 1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j + 1  + N ) % N;
      newWorld[j][i] += oldWorld[row][col];
    }
  }

  // Check each cell to see if it should come to life, continue to live, or die
  int alive = 0;
  
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++) {
      newWorld[j][i] = checkHealth(newWorld[j][i], oldWorld[j][i]);
      alive += newWorld[j][i] ? 1:0;
    }

  return alive;
}
#undef USE
#endif


// When completed this file will contain several versions of the life() function and 
// conditional compilation will be used to determine which version runs.  Each 
// version will be surrounded with  an ifdef, undef, and endif as illustrated 
// immediately below this comment block. To select the version of the life function 
// to use, put the #define USE USE immediately infront of the version of the life() 
// to compile.  Your version of life() must produce a correct result. 

// You are required to document the changes you make in the README.txt file. For each entry
// in the README.txt file there is to be a version of the matching  life() function here such
// that the markers can see, and run if needed,  the actual code that you used 
// to produce your results for the logged change to the life function.  

//Log 3
#ifdef USE
int life(long oldWorld[N][N], long newWorld[N][N]) {

  int i, j, k, l;
  
  //clear the new world
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++) {
      newWorld[j][i] =  0;
    }

  int col, row;
  int q = -1;


  // Count the cells to the top left
  for (i = 0; i < N; i++) {
    col = (i -1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j - 1 + N ) % N;
      newWorld[j][i] += oldWorld[row][col];
      newWorld[j][i] += oldWorld[j][col];
      newWorld[j][i] += oldWorld[row][i];
    }
  }
  
  // Count the cells immediately above
  // for (i = 0; i < N; i++) {
  //   for (j = 0; j < N; j++) {
  //     row = (j - 1 + N ) % N;
      
  //   }
  // }

  // Count the cells to the top right
  for (i = 0; i < N; i++) {
    col = (i + 1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j - 1  + N ) % N;
      newWorld[j][i] += oldWorld[row][col];
    }
  }

 // Count the cells to the immediate left
  // for (i = 0; i < N; i++) {
  //   col = (i -1 + N) % N;
  //   for (j = 0; j < N; j++) {
       
  //   }
  // }
  
  // Count the cells to the immediate right
  for (i = 0; i < N; i++) {
    col = (i + 1 + N) % N;
    for (j = 0; j < N; j++) {
      newWorld[j][i] += oldWorld[j][col];
    }
  }

// Count the cells to the bottom left
  for (i = 0; i < N; i++) {
    col = (i - 1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j + 1 + N ) % N;
      newWorld[j][i] += oldWorld[row][col];
    }
  }

  // Count the cells immediately below
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      row = (j + 1  + N ) % N;
      newWorld[j][i] += oldWorld[row][i];
    }
  }

  // Count the cells to the bottom right
  for (i = 0; i < N; i++) {
    col = (i + 1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j + 1  + N ) % N;
      newWorld[j][i] += oldWorld[row][col];
    }
  }

  // Check each cell to see if it should come to life, continue to live, or die
  int alive = 0;
  
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++) {
      newWorld[j][i] = checkHealth(newWorld[j][i], oldWorld[j][i]);
      alive += newWorld[j][i] ? 1:0;
    }

  return alive;
}
#undef USE
#endif


//Log 2
#ifdef USE
int life(long oldWorld[N][N], long newWorld[N][N]) {

  int i, j, k, l;
  
  //clear the new world
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++) {
      newWorld[j][i] =  0;
    }

  int col, row;
  int q = -1;


  // Count the cells to the top left
  for (i = 0; i < N; i++) {
    col = (i -1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j - 1 + N ) % N;
      newWorld[j][i] += oldWorld[row][col];
      newWorld[j][i] += oldWorld[row][i];
    }
  }
  
  // Count the cells immediately above
  // for (i = 0; i < N; i++) {
  //   for (j = 0; j < N; j++) {
  //     row = (j - 1 + N ) % N;
      
  //   }
  // }

  // Count the cells to the top right
  for (i = 0; i < N; i++) {
    col = (i + 1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j - 1  + N ) % N;
      newWorld[j][i] += oldWorld[row][col];
    }
  }

 // Count the cells to the immediate left
  for (i = 0; i < N; i++) {
    col = (i -1 + N) % N;
    for (j = 0; j < N; j++) {
       newWorld[j][i] += oldWorld[j][col];
    }
  }
  
  // Count the cells to the immediate right
  for (i = 0; i < N; i++) {
    col = (i + 1 + N) % N;
    for (j = 0; j < N; j++) {
      newWorld[j][i] += oldWorld[j][col];
    }
  }

// Count the cells to the bottom left
  for (i = 0; i < N; i++) {
    col = (i - 1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j + 1 + N ) % N;
      newWorld[j][i] += oldWorld[row][col];
    }
  }

  // Count the cells immediately below
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      row = (j + 1  + N ) % N;
      newWorld[j][i] += oldWorld[row][i];
    }
  }

  // Count the cells to the bottom right
  for (i = 0; i < N; i++) {
    col = (i + 1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j + 1  + N ) % N;
      newWorld[j][i] += oldWorld[row][col];
    }
  }

  // Check each cell to see if it should come to life, continue to live, or die
  int alive = 0;
  
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++) {
      newWorld[j][i] = checkHealth(newWorld[j][i], oldWorld[j][i]);
      alive += newWorld[j][i] ? 1:0;
    }

  return alive;
}
#undef USE
#endif






// For each version of life you are testing duplicate the code between the 
// ifdef and endif and make your changes. To use the new version, move the the #define 
// just about this comment to be just in front of the ifdef for the new version.
//ORIGINAL
#ifdef USE

int life(long oldWorld[N][N], long newWorld[N][N]) {

  int i, j, k, l;
  
  //clear the new world
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++) {
      newWorld[j][i] =  0;
    }

  int col, row;
  int q = -1;


  // Count the cells to the top left
  for (i = 0; i < N; i++) {
    col = (i -1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j - 1 + N ) % N;
      newWorld[j][i] += oldWorld[row][col];
    }
  }
  
  // Count the cells immediately above
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      row = (j - 1 + N ) % N;
      newWorld[j][i] += oldWorld[row][i];
    }
  }

  // Count the cells to the top right
  for (i = 0; i < N; i++) {
    col = (i + 1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j - 1  + N ) % N;
      newWorld[j][i] += oldWorld[row][col];
    }
  }

 // Count the cells to the immediate left
  for (i = 0; i < N; i++) {
    col = (i -1 + N) % N;
    for (j = 0; j < N; j++) {
       newWorld[j][i] += oldWorld[j][col];
    }
  }
  
  // Count the cells to the immediate right
  for (i = 0; i < N; i++) {
    col = (i + 1 + N) % N;
    for (j = 0; j < N; j++) {
      newWorld[j][i] += oldWorld[j][col];
    }
  }

// Count the cells to the bottom left
  for (i = 0; i < N; i++) {
    col = (i - 1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j + 1 + N ) % N;
      newWorld[j][i] += oldWorld[row][col];
    }
  }

  // Count the cells immediately below
  for (i = 0; i < N; i++) {
    for (j = 0; j < N; j++) {
      row = (j + 1  + N ) % N;
      newWorld[j][i] += oldWorld[row][i];
    }
  }

  // Count the cells to the bottom right
  for (i = 0; i < N; i++) {
    col = (i + 1 + N) % N;
    for (j = 0; j < N; j++) {
      row = (j + 1  + N ) % N;
      newWorld[j][i] += oldWorld[row][col];
    }
  }

  // Check each cell to see if it should come to life, continue to live, or die
  int alive = 0;
  
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++) {
      newWorld[j][i] = checkHealth(newWorld[j][i], oldWorld[j][i]);
      alive += newWorld[j][i] ? 1:0;
    }

  return alive;
}
#undef USE
#endif
