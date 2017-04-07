licenses(['notice'])
package(default_visibility = ['//visibility:public'])

cc_library(
        name = 'lib',  # libicui18n
        srcs = glob([
                '**/*.c',
                '**/*.cpp',
        ]),
        hdrs = glob([
                '**/*.h',
        ]),
        deps = [
                '//third_party/icu/common:lib',
        ],
        copts = [
                '-fno-exceptions',
                '-iquote third_party/icu/common',
                '-iquote third_party/icu/i18n',
                '-DU_I18N_IMPLEMENTATION',
        ],
)
