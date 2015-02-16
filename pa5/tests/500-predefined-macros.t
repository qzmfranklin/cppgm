#if __CPPGM__ == 201304L
	wrong_course_run 1
#elif __CPPGM__ == 201303L
	right_course_run
#else
	wrong_course_run 2
#endif

#if __cplusplus == 201104L
	wrong_cplusplus_version 1
#elif __cplusplus == 201103L
	right_cplusplus_version
#else
	wrong_cplusplus_version 2
#endif

#if __STDC_HOSTED__ == 2
	wrong__STDC_HOSTED_value 1
#elif __STDC_HOSTED__ == 1
	right__STDC_HOSTED_value
#else
	wrong__STDC_HOSTED_value 2
#endif

#if __LINE__ == 26 /* <--- be careful with this tests line position */
	wrong__LINE__ 1
#elif __LINE__ == 27
	right__LINE__
#else
	wrong__LINE__ 2
#endif

