#include"rocc.h"
#include <riscv-pk/encoding.h>
#include <stdio.h>
#include "marchid.h"
#include "spin_lock.h"
static size_t n_cores = 8;
int uart_lock;
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

static inline void custom_jalr()
{
	R_INSTRUCTION_JLR(3, 0x01);
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
  case 0x03:
    custom_jalr();//0x02001067
  break;

  default:
    break;
  }

  return 0;
}


int main(void){
  transmission(1); //0x0205A00B
  
  
  accum_write(0, 1);//0x00C5B00B
  accum_write(0, 2);
  accum_write(0, 3);
  accum_write(0, 4);
  accum_write(0, 5);
  accum_write(0, 6);
  accum_write(0, 7);
  accum_write(0, 8);
  accum_write(0, 9);
  accum_write(0, 10);
  
  lock_acquire(&uart_lock);
  printf("helloworld\n");
  lock_release(&uart_lock);
  return 0;
}
