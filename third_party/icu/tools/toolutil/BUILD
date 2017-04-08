licenses(['notice'])
package(default_visibility = ['//visibility:public'])

cc_library(
        name = 'lib', # libicutu
        srcs = glob([
                '*.c',
                '*.cpp',
        ]),
        hdrs = glob([
                '*.h',
        ]),
        deps = [
                '//third_party/icu/i18n:lib',
        ],
        copts = [
                '-fno-exceptions',
                '-I third_party/icu/common',
                '-I third_party/icu/i18n',
                '-I third_party/icu/tools/toolutil',
                '-DU_TOOLUTIL_IMPLEMENTATION',
        ],
)
