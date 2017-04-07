licenses(['notice'])
package(default_visibility = ['//visibility:public'])

cc_library(
        name = 'io',
        srcs = glob([
                'io/**/*.c',
                'io/**/*.cpp',
        ]),
        hdrs = glob([
                'io/**/*.h',
        ]),
        deps = [
                'common',
        ],
        copts = [
                '-iquote third_party/icu4c/icu/source/common',
                '-fno-exceptions',
                '-DU_IO_IMPLEMENTATION',
        ],
)

cc_library(
        name = 'common',
        srcs = glob([
                'common/**/*.c',
                'common/**/*.cpp',
        ]),
        hdrs = glob([
                'common/**/*.h',
        ]),
        copts = [
                '-iquote third_party/icu4c/icu/source/common',
                '-fno-exceptions',
                '-DU_COMMON_IMPLEMENTATION',
        ],
        visibility = ["//visibility:__pkg__"],
)
