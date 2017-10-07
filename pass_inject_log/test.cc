
#include <stdio.h>
#include <time.h>


int global_number = 10;


//test we put traces also for functions which are optimized out
void more_inner(int a) {
}

void g2(long a) {
  if (a%2 == 0)  {more_inner(a); printf("x1\n"); return;}
  if (a%10 == 5) {more_inner(a); printf("x2\n"); return;}
}

struct c1_t {
  c1_t() {
  }
  void t1() {
    printf("in method\n");
  }
};

int main()
{
  c1_t c;
  c.t1();
  g2(time(NULL));
  for (int i=0; i<10; i++)
    printf("this is test\n");
  printf("%ld\n", time(NULL));
  printf("%d\n", global_number);
}



