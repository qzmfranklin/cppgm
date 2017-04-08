licenses(['notice'])
package(default_visibility = ['//visibility:public'])

cc_binary(
        name = 'makeconv',
        srcs = glob([
                '*.c',
                '*.cpp',
                '*.h',
        ]),
        deps = [
                '//third_party/icu/stubdata:lib',
                '//third_party/icu/i18n:lib',
                '//third_party/icu/tools/toolutil:lib',
        ],
        copts = [
                '-I third_party/icu/common',
                '-I third_party/icu/tools/toolutil',

        ],
        linkopts = [
                '-ldl',
        ],
)


#clang++ -O3  -W -Wall -pedantic -Wpointer-arith -Wwrite-strings -Wno-long-long --std=c++0x  -Qunused-arguments -Wno-parentheses-equality -Wl,--gc-sections   -o ../../bin/makeconv makeconv.o ucnvstat.o genmbcs.o gencnvex.o -L../../lib -licutu -L../../lib -licui18n -L../../lib -licuuc -L../../stubdata -licudata -lpthread -ldl -lm
