
# The gflags library can be used with bazel per the following instruction:
#       https://github.com/gflags/gflags/blob/master/INSTALL.md
# Refer to gflags as:
#       //external:gflags
git_repository(
        name = "com_github_gflags_gflags",
        commit = "9314597d4b742ed6f95665241345e590a0f5759b",
        remote = "https://github.com/gflags/gflags.git",
)

bind(
        name = "gflags",
        actual = "@com_github_gflags_gflags//:gflags",
)
