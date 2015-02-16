#define foo 1

#ifdef foo
right_foo
#else
wrong_foo
#endif

#ifdef bar
wrong_bar
#else
right_bar
#endif

#ifdef bar
wrong_foobar1
#elif defined foo
right_foobar
#else
wrong_foobar2
#endif

#ifdef __CPPGM__
right___CPPGM__
#else
wrong___CPPGM__
#endif

#ifdef __CPPGM_AUTHOR__
right___CPPGM_AUTHOR___
#else
wrong___CPPGM_AUTHOR___
#endif

#ifdef __cplusplus
right___cpluscplus
#else
wrong___cplusplus
#endif

#ifdef __STDC_HOSTED__
right___STDC_HOSTED__
#else
wrong___STDC_HOSTED__
#endif

#ifdef __FILE__
right___FILE__
#else
wrong___FILE__
#endif

#ifdef __LINE__
right___LINE__
#else
wrong___LINE__
#endif

#ifdef __DATE__
right___DATE__
#else
wrong___DATE__
#endif

#ifdef __TIME__
right___TIME__
#else
wrong___TIME__
#endif

 # ifdef foo
   # ifndef bar
     #ifdef __CPPGM__
       #   ifdef __CPPGM_AUTHOR__
         	#ifdef __cplusplus
			#ifdef __STDC_HOSTED__
				#if defined(__FILE__) && defined(__DATE__) && defined(__TIME__)
					YAY
				#endif
			#endif
		#endif
	#endif
	#  endif
   #	endif
#endif

 # ifdef foo
   # ifndef bar
     #ifdef __CPPGM__
       #   ifdef __CPPGM_AUTHOR__
         	#ifndef __cplusplus // <--- different
			#ifdef __STDC_HOSTED__
				#if defined(__FILE__) && defined(__DATE__) && defined(__TIME__)
					BOO
				#endif
			#endif
		#endif
	#endif
	#  endif
   #	endif
#endif
