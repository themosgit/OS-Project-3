#include "utils.h"


struct circularbuff{
  uint8_t buffer[100];
  size_t head;
  size_t tail;
  size_t max;

}; 

struct sharedmem {

  CircularBuff buffer;

  int NumOfWaters;
  int NumOfCheeses;
  int NumOfWines;
  int NumOfSalads;
  int AvgVisitDuration;
  int NumberOfVisitors;
  int AvgWaitingTime;
};




