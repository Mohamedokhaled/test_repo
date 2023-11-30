#ifndef _SYS_TRACE_H_
#define _SYS_TRACE_H_


#define CONFIG_TRACE   1

#define CONFIG_TRACE_1 1
#define CONFIG_TRACE_2 2
#define CONFIG_TRACE_3 3
#define CONFIG_TRACE_4 4

void sysal_trace(const char *fmt, ...);

#define ptrace(cond, fmt, ...) do {          \
	if(!cond)                            \
		sysal_trace(fmt, ##__VA_ARGS__); \
} while(0)

#define etrace(fmt, ...) sysal_trace(fmt, ##__VA_ARGS__)
#define itrace(fmt, ...)
#define wtrace(fmt, ...)
#define dtrace(fmt, ...)
#define vtrace(fmt, ...)

#if (CONFIG_TRACE >= CONFIG_TRACE_1)
	#undef itrace
	#define itrace(fmt, ...) sysal_trace(fmt, ##__VA_ARGS__)
#endif
#if (CONFIG_TRACE >= CONFIG_TRACE_2)
	#undef wtrace
	#define wtrace(fmt, ...) sysal_trace(fmt, ##__VA_ARGS__)
#endif

#if (CONFIG_TRACE >= CONFIG_TRACE_3)
	#undef dtrace
	#define dtrace(fmt, ...) sysal_trace(fmt, ##__VA_ARGS__)
#endif

#if (CONFIG_TRACE >= CONFIG_TRACE_4)
	#undef vtrace
	#define vtrace(fmt, ...) sysal_trace(fmt, ##__VA_ARGS__)
#endif

#endif /* _SYS_TRACE_H_ */
