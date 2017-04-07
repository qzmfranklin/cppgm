licenses(['notice'])
package(default_visibility = ['//visibility:public'])

cc_library(
        name = 'icu',
        deps = [
                'io',
                'i18n',
                'le',
                'lx',
                'uc',
        ],
)

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
                '-fno-exceptions',
                '-iquote third_party/icu4c/icu/source/common',
                '-DU_IO_IMPLEMENTATION',
        ],
)

cc_library(
        name = 'i18n',
        srcs = glob([
                'i18n/**/*.c',
                'i18n/**/*.cpp',
        ]),
        hdrs = glob([
                'i18n/**/*.h',
        ]),
        deps = [
                'common',
        ],
        copts = [
                '-fno-exceptions',
                '-iquote third_party/icu4c/icu/source/common',
                '-iquote third_party/icu4c/icu/source/i18n',
                '-DU_I18N_IMPLEMENTATION',
        ],
)

cc_library(
        name = 'le',
        srcs = glob([
                'le/**/*.c',
                'le/**/*.cpp',
        ]),
        hdrs = glob([
                'le/**/*.h',
        ]),
        copts = [
                '-fno-exceptions',
        ],
)

cc_library(
        name = 'lx',
        srcs = glob([
                'lx/**/*.c',
                'lx/**/*.cpp',
        ]),
        hdrs = glob([
                'lx/**/*.h',
        ]),
        copts = [
                '-fno-exceptions',
        ],
)

cc_library(
        name = 'uc',
        srcs = glob([
                'uc/**/*.c',
                'uc/**/*.cpp',
        ]),
        hdrs = glob([
                'uc/**/*.h',
        ]),
        copts = [
                '-fno-exceptions',
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
                '-fno-exceptions',
                '-iquote third_party/icu4c/icu/source/common',
                '-DU_COMMON_IMPLEMENTATION',
        ],
        visibility = ["//visibility:__pkg__"],
)
