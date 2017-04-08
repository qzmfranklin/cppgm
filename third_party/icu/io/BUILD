licenses(['notice'])
package(default_visibility = ['//visibility:public'])

cc_library(
        name = 'lib', # libicuio
        srcs = glob([
                '*.c',
                '*.cpp',
        ]),
        hdrs = glob([
                '**/*.h',
        ]),
        deps = [
                '//third_party/icu/common:lib',
        ],
        copts = [
                '-fno-exceptions',
                '-I third_party/icu/common',
                '-DU_IO_IMPLEMENTATION',
        ],
)
