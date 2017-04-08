licenses(['notice'])
package(default_visibility = ['//visibility:public'])

cc_library(
        name = 'lib', # libicutest
        srcs = glob([
                '*.c',
                '*.cpp',
        ]),
        hdrs = glob([
                '**/*.h',
        ]),
        deps = [
                '//third_party/icu/common:lib',
                '//third_party/icu/tools/toolutil:lib',
        ],
        copts = [
                '-fno-exceptions',
                '-I third_party/icu/common',
                '-I third_party/icu/tools/ctestfw',
                '-I third_party/icu/tools/toolutil',
        ],
)
