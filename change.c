#include"rocc.h"
#include <riscv-pk/encoding.h>
#include <stdio.h>
#include "marchid.h"
#include "spin_lock.h"
static size_t n_cores = 8;
uint8_t lock[7] = {0};
static void __attribute__((noinline)) barrier()
{
  static volatile int sense;
  static volatile int count;
  static __thread int threadsense;

  __sync_synchronize();

  threadsense = !threadsense;
  if (__sync_fetch_and_add(&count, 1) == n_cores-1)
  {
    count = 0;
    sense = threadsense;
  }
  else while(sense != threadsense)
    ;

  __sync_synchronize();
}

static inline void accum_write(int idx, unsigned long data)
{
	ROCC_INSTRUCTION_SS(0, data, idx, 0);
}

static inline void change(int num, unsigned long HartID)
{
	ROCC_INSTRUCTION_SS(0, num, HartID, 4);
}

static inline void transmission(int valid)
{
  ROCC_INSTRUCTION_S(0, valid, 1);
}

static inline void receive(int ready)
{
  ROCC_INSTRUCTION_S(0, ready, 2);
}

unsigned long num1 = 0x32L;
unsigned long HartID1 = 0x45762310L;

unsigned long num2 = 0x21L;
unsigned long HartID2 = 0x74561230L;

unsigned long data = 0x3421L;

void idle()
{
  while(1){};
}

int __main(void){
	uint64_t Hart_id = 0;
  asm volatile ("csrr %0, mhartid"  : "=r"(Hart_id));

  switch (Hart_id)
  {
  case 0x01:
    change(num1, HartID1);
    lock[1] = 1;
    accum_write(0, 3);
    while(lock[0] != 1 || lock[1] != 1 || lock[2] != 1 || lock[3] != 1 || lock[4] != 1 || lock[5] != 1 || lock[6] != 1 || lock[7] != 1){}
    receive(1);
    barrier();
  break;

  case 0x02:
    change(num1, HartID1);
    lock[2] = 1;
    accum_write(0, 3);
    while(lock[0] != 1 || lock[1] != 1 || lock[2] != 1 || lock[3] != 1 || lock[4] != 1 || lock[5] != 1 || lock[6] != 1 || lock[7] != 1){}
    transmission(1);
    barrier();
  break;

  case 0x03:
    change(num1, HartID1);
    lock[3] = 1;
    accum_write(0, 3);
    while(lock[0] != 1 || lock[1] != 1 || lock[2] != 1 || lock[3] != 1 || lock[4] != 1 || lock[5] != 1 || lock[6] != 1 || lock[7] != 1){}
    transmission(1);
    barrier();
  break;
  
  case 0x04:
    change(num1, HartID1);
    lock[4] = 1;
    accum_write(0, 3);
    while(lock[0] != 1 || lock[1] != 1 || lock[2] != 1 || lock[3] != 1 || lock[4] != 1 || lock[5] != 1 || lock[6] != 1 || lock[7] != 1){}
    transmission(1);
    barrier();
  break;

  case 0x05:
    change(num1, HartID1);
    lock[5] = 1;
    accum_write(0, 3);
    barrier();
  break;

  case 0x06:
    change(num1, HartID1);
    lock[6] = 1;
    accum_write(0, 3);
    while(lock[0] != 1 || lock[1] != 1 || lock[2] != 1 || lock[3] != 1 || lock[4] != 1 || lock[5] != 1 || lock[6] != 1 || lock[7] != 1){}
    receive(1);
    barrier();
  break;

  case 0x07:
    change(num1, HartID1);
    lock[7] = 1;
    accum_write(0, 3);
    barrier();
  break;

  default:
    break;
  }

  return 0;
}

int main(void){
  change(num1, HartID1);
  lock[0] = 1;
  accum_write(0, 3);
  while(lock[0] != 1 || lock[1] != 1 || lock[2] != 1 || lock[3] != 1 || lock[4] != 1 || lock[5] != 1 || lock[6] != 1 || lock[7] != 1){}
  receive(1);
  //R_INSTRUCTION_JLR(3, 0x00);
	return 0;
}
