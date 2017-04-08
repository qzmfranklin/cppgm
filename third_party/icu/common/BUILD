licenses(['notice'])
package(default_visibility = ['//visibility:public'])

cc_library(
        name = 'lib',  # uc
        srcs = glob([
                '*.c',
                '*.cpp',
        ]),
        hdrs = glob([
                '**/*.h',
        ]),
        copts = [
                '-fno-exceptions',
                '-I third_party/icu/common',
                '-DU_COMMON_IMPLEMENTATION',
        ],
)
