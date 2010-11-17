#define _GNU_SOURCE

#include <asm/ldt.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define LDT_READ                    0
#define LDT_WRITE                   1
#define LDT_READ_DEFAULT_LDT        2
#define LDT_WRITE_DEFAULT_LDT    0x11


unsigned long mx __attribute__((section(".michaelx"))) = 0xDEADBEEF;
unsigned long my __attribute__((section(".michaely"))) = 0xC001BEEF;

int main(int argc,
	 char* argv[])
{
  unsigned int entry_num;
  unsigned long base_addr;
  unsigned int limit;
  unsigned int seg_32bit;
  unsigned int dpl;

  int num_bytes = 0;
  int done = 1; //1==FALSE
  //struct user_desc table_entry[NUM];
  struct user_desc* table_entry_ptr = NULL;
 
  /* Allocates memory for a user_desc struct */ 
  table_entry_ptr = (struct user_desc*)malloc(sizeof(struct user_desc));
  if(NULL==table_entry_ptr)
  {
    fprintf(stderr,
	    "malloc failed\n");
    return -1;
  }

  fprintf (stdout,
	   "Size of the user_desc struct is: %i\n",
	   sizeof(struct user_desc));

  while(1==done)
  {
    /* Checks to see if anything is in the ldt */ 
    num_bytes = syscall( __NR_modify_ldt,
			 LDT_READ,
			 table_entry_ptr,
			 sizeof(struct user_desc)
			 );
    if(-1==num_bytes)
    {
      perror("reading");
      return -1;
    }else if(0==num_bytes){
      fprintf(stdout,"no LDT entries\n");
    }else{
      fprintf(stdout,
	      "ldt entry(%x) %u %u %u\n",
	      ((unsigned int)table_entry_ptr),
	      table_entry_ptr->entry_number,
	      table_entry_ptr->base_addr,
	      table_entry_ptr->limit);
    }
    if(num_bytes==0)
      done = 0;
  }
  
  fprintf(stdout,
	  "done with first read\n");
  fflush(stdout);

  /* Fills the user_desc struct *
     Represents segment for mx  */
  table_entry_ptr->entry_number = 0;
  table_entry_ptr->base_addr = ((unsigned long)&mx); 
  table_entry_ptr->limit = 0x4; 
  table_entry_ptr->seg_32bit = 0x1; 
  table_entry_ptr->contents = 0x0;  
  table_entry_ptr->read_exec_only = 0x0; 
  table_entry_ptr->limit_in_pages = 0x1; 
  table_entry_ptr->seg_not_present = 0x0;
  table_entry_ptr->useable = 0x1; 
  table_entry_ptr->dpl_prot = 0x2;

  fprintf(stdout,
	  "storing location of mx: %p\n",
	  &mx);

  /* Writes a user_desc struct to the ldt */
  num_bytes = syscall( __NR_modify_ldt,
		       LDT_WRITE,
		       table_entry_ptr,
		       sizeof(struct user_desc)
		       );

  if(-1==num_bytes)
  {
    fprintf(stderr,
	    "failed to write\n");
    perror("writing");
    return -1;
  }else if(0==num_bytes){
    fprintf(stdout,
	    "wrote to our LDT\n");
    fflush(stdout);
  }

  /* Fills the user_desc struct    *
     Represents the segment for my */
  /*table_entry_ptr->entry_number = 1;
  table_entry_ptr->base_addr = ((unsigned long)&my); 
  table_entry_ptr->limit = 0x4;
  table_entry_ptr->seg_32bit = 0x1;
  table_entry_ptr->contents = 0x2;
  table_entry_ptr->read_exec_only = 0x1;
  table_entry_ptr->limit_in_pages = 0x1;
  table_entry_ptr->seg_not_present = 0x1;
  table_entry_ptr->useable = 0x1;
  table_entry_ptr->dpl_prot = 2;
  */
  fprintf(stdout,
	  "storing location of my: %p\n",
	  &my);

  /*num_bytes = syscall( __NR_modify_ldt,
		       LDT_WRITE,
		       table_entry_ptr,
		       sizeof(struct user_desc)
		       );

  if(-1==num_bytes)
  {
    fprintf(stderr,
	    "failed to write\n");
    perror("writing");
    return -1;
  }else if(0==num_bytes){
    fprintf(stdout,
	    "wrote to our LDT\n");
    fflush(stdout);
    }*/

  /* Clears the user_desc struct */
  table_entry_ptr->entry_number = 0;
  table_entry_ptr->base_addr = 0x0;
  table_entry_ptr->limit = 0;
  table_entry_ptr->seg_32bit = 0;  
  table_entry_ptr->contents = 0x0;
  table_entry_ptr->read_exec_only = 0;
  table_entry_ptr->limit_in_pages = 0;
  table_entry_ptr->seg_not_present = 0;
  table_entry_ptr->useable = 0;
  table_entry_ptr->dpl_prot = 0;

  /* Reads the ldt */
  num_bytes = syscall( __NR_modify_ldt,
		       LDT_READ,
		       table_entry_ptr,
		       sizeof(struct user_desc)
		       );

  int i = 0;
  char* v = NULL;
  v = ((char*)table_entry_ptr);

  /* Prints out whats in the table_entry_ptr (the ldt) byte by byte */
  fprintf(stdout,
	  "The user_desc: \n");
  for(i=0;i<num_bytes;i++)
  {
    fprintf(stdout,
	    "%x\n",
	    *v);
    v++;
    }
  
  /* Prints the information from the first user_desc struct */
  struct user_desc d = (struct user_desc)(*table_entry_ptr);
  entry_num = d.entry_number;
  base_addr = d.base_addr;
  limit = d.limit;
  seg_32bit = d.seg_32bit;
  dpl = d.dpl_prot;

  fprintf(stdout,
	  "read %d bytes; entry(%x) base:%lx limit:%u seg_32bit:%u dpl:%u\n",
	  num_bytes,
	  entry_num,
	  base_addr,
	  limit,
	  seg_32bit,
	  dpl);

  /*d = (struct user_desc)(*(table_entry_ptr++));
  entry_num = d.entry_number;
  base_addr = d.base_addr;
  limit = d.limit;
  seg_32bit = d.seg_32bit;

  fprintf(stdout,
	  "read %d bytes; entry(%x) %lx %u %u\n",
	  num_bytes,
	  entry_num,
	  base_addr,
	  limit,
	  seg_32bit);
  */
  /*
  num_bytes = syscall( __NR_modify_ldt,
		       LDT_READ_DEFAULT_LDT,
		       &table_entry,
		       (NUM*sizeof(struct user_desc))
		       );

  fprintf(stdout,
	  "big read: read %d bytes\n",
	  num_bytes);

  i = 0;
  v = NULL;
  v = ((char*)&table_entry);
  for(i=0;i<num_bytes;i++)
  {
    fprintf(stdout,
	    "%x\n",
	    *v);
    v++;
    }*/
  

  /* modify these variables in their sections. should produce ASM
   * with the "correct" addresses to modify
   */
  
  mx = 0x407BABE;
  my = 0xC001CAFE;

  mx = 0x3424BABE;
  my = 0x2300C001;

  printf ("Mx is: %lx\n", mx);
  printf ("My is: %lx\n", my);

  return 0;
}
