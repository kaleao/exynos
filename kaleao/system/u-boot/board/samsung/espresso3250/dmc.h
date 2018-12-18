#ifndef _DMC_H_
#define _DMC_H_

void InitDMC(u32 MemClk, RST_STAT eRstStat);

typedef enum {
  LPDDR3,
  DDR3,
  LPDDR2,
}Memory_Type;

#endif /* _DMC_H_ */
