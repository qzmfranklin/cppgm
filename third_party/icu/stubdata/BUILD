licenses(['notice'])
package(default_visibility = ['//visibility:public'])

cc_library(
        name = 'data', # libicudata
        srcs = glob([
                '*.c',
        ]),
        deps = [
                '//third_party/icu/common:lib',
        ],
        copts = [
                '-fno-exceptions',
                '-I third_party/icu/common',
        ],
)
