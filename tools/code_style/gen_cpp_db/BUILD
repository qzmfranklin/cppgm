action_listener(
    name = 'gen_cxx_cmd_listener',
    visibility = ['//visibility:public'],
    mnemonics = [
        'CppCompile',
    ],
    extra_actions = [':gen_cxx_cmd_action'],
)

extra_action(
    name = 'gen_cxx_cmd_action',
    tools = [
        ':action2cxxcmd',
    ],
    out_templates = [
        '$(ACTION_ID)_cpp_compile_command',
    ],
    cmd = ' '.join([
        '$(location :action2cxxcmd)',
        '$(EXTRA_ACTION_FILE)',
        '$(output $(ACTION_ID)_cpp_compile_command)',
    ]),
)

py_binary(
    name = 'action2cxxcmd',
    srcs = [
        'action2cxxcmd.py',
    ],
    deps = [
        ':extra_actions_base_proto_py',
    ],
)

py_library(
    name = 'extra_actions_base_proto_py',
    srcs = [
        'extra_actions_base_pb2.py',
    ],
)
