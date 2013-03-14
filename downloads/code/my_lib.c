/*  Questa libreria racchiude le funzioni di uso generico non strettamente */
/*  correlate con IPC. */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


/*  Questa funzione ha lo scopo di generare dei numeri casuali tra 0 e max. */
/*  Viene data la possibilità di decidere il seme della generazione dei numeri */
/*  pseudo-casuali, ma ad essa viene aggiunto il risultato della funzione time */
/*  che restituisce il numero di secondi che sono passati dall'Epoca (00:00:00 */
/*  1 gennaio 1970) in modo da garantire che ad ogni esecuzione il numero */
/*  estratto sia diverso anche impostando lo stesso seme. */

int init_rand(unsigned int seed){
  srandom(seed);
}

int myrand(int max)
{
  double r,x;
  r = (double) random();
  x = r * (double) max / RAND_MAX;
  return((int) x);
}



