#include <bmk-core/memalloc.h>
#include <bmk-core/types.h>
#include <bmk-core/null.h>
#include <bmk-core/core.h>
#include <bmk-core/jsmn.h>
#include <bmk-core/sched.h>
#include <execinfo.h>
//#include <bmk/kernel.h>

#include <arch/i386/types.h>

#include <rumpcalls.h>


// THESE MIGHT NEED TO BE DEFINED FIXMEE

typedef long long bmk_time_t;

unsigned long bmk_stackpageorder;
unsigned long bmk_stacksize;

unsigned long bmk_pagesize = 4096;
unsigned long bmk_pageshift = 12;

unsigned long bmk_memsize;

struct cos_rumpcalls crcalls;

void* _GLOBAL_OFFSET_TABLE_ = (void *) 0x1337BEEF;


/* Prototypes */

struct bmk_thread;

struct bmk_tcb;

struct lwp *bmk_curlwp(void);
void  bmk_printf(const char *fmt, ...);
int bmk_snprintf(char *bf, unsigned long size, const char *fmt, ...);
bmk_time_t bmk_platform_clock_monotonic(void);
bmk_time_t bmk_platform_clock_epochoffset(void);
void __attribute__((noreturn)) bmk_platform_halt(const char *panicstring);
int bmk_strcmp(const char *a, const char *b);
void *bmk_memset(void *b, int c, unsigned long n);
void *bmk_memcpy(void *d, const void *src, unsigned long n);
void *bmk_pgalloc(int order);
void  bmk_pgfree(void *pointer, int order);
void  bmk_vprintf(const char *fmt, va_list ap);
char *bmk_strncpy(char *d, const char *s, unsigned long n);
int   rumprun_platform_rumpuser_init(void);
int   bmk_isr_init(int (*func)(void *), void *arg, int intr);

void print_trace(void);

void bmk_platform_cpu_sched_settls(struct bmk_tcb *next);
unsigned long bmk_platform_splhigh(void);
void bmk_platform_block(bmk_time_t until);
void bmk_cpu_sched_create(struct bmk_thread *thread, struct bmk_tcb *tcb, void (*f)(void *), void *arg, void *stack_base, unsigned long stack_size);
char * bmk_strcpy(char *d, const char *s);
void bmk_platform_splx(unsigned long);
void bmk_cpu_sched_switch_viathd(struct bmk_thread *prev, struct bmk_thread *next);
/* Prototype Definitions */

void
bmk_printf(const char *fmt, ...)
{
	char s[128];
	va_list arg_ptr;
	int ret, len = 128;

	va_start(arg_ptr, fmt);
	ret = crcalls.rump_vsnprintf(s, len, fmt, arg_ptr);
	va_end(arg_ptr);
	crcalls.rump_cos_print(s, ret);

	return;
}

int
bmk_snprintf(char *bf, unsigned long size, const char *fmt, ...)
{
	bmk_printf("bmk_snprintf:\n");

	int ret;
	va_list arg_ptr;

	va_start(arg_ptr, fmt);
	ret = crcalls.rump_vsnprintf(bf, size, fmt, arg_ptr);
	va_end(arg_ptr);

	return ret;
}

void
bmk_platform_cpu_sched_settls(struct bmk_tcb *next)
{
	bmk_printf("bmk_platform_cpu_sched_settls is being called / ignored\n");
	return;
}

unsigned long
bmk_platform_splhigh(void)
{
	bmk_printf("bmk_platform_splhigh is being called/ignored\n");
//	while(1);
	return 0;
}

void
bmk_platform_block(bmk_time_t until)
{
	bmk_printf("bmk_platform_block is being called\n");
	while(1);
}

void
bmk_platform_splx(unsigned long x)
{
	bmk_printf("bmk_platform_splx is being called/ignored\n");
	//while(1);
}

void
bmk_cpu_sched_create(struct bmk_thread *thread, struct bmk_tcb *tcb,
        void (*f)(void *), void *arg,
        void *stack_base, unsigned long stack_size)
{

	/*
	 * In the composite related function we want to:
	 * We need to get the thdid so we can set up bmk_current thd
	 * later on *initcurrent*, We want to assign thdcap_t within the
	 * thread that is being passed in.
	 * */
	bmk_printf("bmk_cpu_sched_create is being called\n");

	crcalls.rump_cpu_sched_create(thread, tcb, f, arg, stack_base, stack_size);
	bmk_printf("bmk_cpu_sched_create has been called\n");
}

char *
bmk_strcpy(char *d, const char *s)
{
	bmk_printf("bmk_strcpy is being called\n");
	bmk_printf("TODO: implement with composite's strcpy instead\n");

	char *orig = d;

	while ((*d++ = *s++) != '\0')
		continue;
	return orig;
}

void
bmk_cpu_sched_switch_viathd(struct bmk_thread *prev, struct bmk_thread *next)
{
	bmk_printf("SCHED: bmk_cpu_sched_switch_viathd is being called\n");

	crcalls.rump_cpu_sched_switch_viathd(prev, next);
	bmk_printf("SCHED: bmk_cpu_sched_switch_viathd has been called\n");
}

bmk_time_t
bmk_platform_clock_monotonic(void)
{
	bmk_printf("bmk_platform_clock_monotonic is being called.\n");

	return 0;
}

bmk_time_t
bmk_platform_clock_epochoffset(void)
{
	bmk_printf("bmk_platform_clock_epochoffset is being called.\n");

	return 0;
}

void __attribute__((noreturn))
bmk_platform_halt(const char *panicstring)
{
	bmk_printf("bmk_platform_halt is being called.\n");
	bmk_printf("It's message is: ");
	bmk_printf(panicstring);
	while(1);
}

int
bmk_strcmp(const char *a, const char *b)
{
	bmk_printf("bmk_strcmp is being called.\n");

	int rv;

	rv = crcalls.rump_strcmp(a, b);

	return rv;
}

/*void 
print_trace (void)
{
	void *array[10];
	size_t size;
	char **strings;
	size_t i;

	size = backtrace(array, 10);
	strings = backtrace_symbols(array, size);

	for(i=0; i < size; i++)
		bmk_printf("%s\n", strings[i]);

}*/

void *
bmk_memset(void *b, int c, unsigned long n)
{
	bmk_printf("bmk_memset is being called.\n");
	unsigned char *v = b;

	while (n--)
		*v++ = (unsigned char)c;

	return b;

	// Changed in response to malloc bug. See notes
	//crcalls.rump_memset(b, c, n);
	//return b;
}

void *
bmk_memcpy(void *d, const void *src, unsigned long n)
{
	bmk_printf("bmk_memcpy is being called.\n");

	void *ret;

	ret = crcalls.rump_memcpy(d, src, n);

	return ret;
}


void
bmk_vprintf(const char *fmt, va_list ap)
{

	char s[128];
	int ret, len = 128;

	//va_start(ap, fmt);
	ret = crcalls.rump_vsnprintf(s, len, fmt, ap);
	//va_end(ap);
	crcalls.rump_cos_print(s, ret);

	return;

}

char *
bmk_strncpy(char *d, const char *s, unsigned long n)
{
	bmk_printf("bmk_strncpy is being called.\n");

	char *rv;

	rv = crcalls.rump_strncpy(d, s, n);

	return rv;
}

/* RG:
 * This simply returns 0 within the hw implementation.
 * The xen implementation uses this to begin bio
 */

int
rumprun_platform_rumpuser_init(void)
{
	bmk_printf("rumprun_platform_rumpuser_init is being called.\n");
	return 0;
}

int
bmk_isr_init(int (*func)(void *), void *arg, int intr)
{
	bmk_printf("bmk_isr_init is being called.\n");
	while(1);
	return 0;
}

void
jsmn_init(jsmn_parser *parser)
{
	bmk_printf("jsmn_init unimplemented");
}

jsmnerr_t
jsmn_parse(jsmn_parser *parser, const char *js, unsigned long len, jsmntok_t *tokens, unsigned int num_tokens)
{
	bmk_printf("jsmn_parse unimplemented");
	return 0;
}