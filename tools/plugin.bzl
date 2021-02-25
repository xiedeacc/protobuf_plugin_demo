def _get_external_root(ctx):
    gendir = ctx.var["GENDIR"] + "/"
    print(gendir)
    external_roots = []
    path = ctx.attr.src.files.to_list()[0].path
    if path.startswith(gendir):
        path = path[len(gendir):]
    path = path.split("/")
    if path[0] == "external":
        external_roots += ["/".join(path[0:2])]
    roots = depset(external_roots)
    n = len(roots.to_list())
    if n > 1:
        fail("""
           You are attempting simultaneous compilation of protobuf source files that span multiple workspaces (%s).
           Decompose your library rules into smaller units having filesets that belong to only a single workspace at a time.
           Note that it is OK to *import* across multiple workspaces, but not compile them as file inputs to protoc.
           """ % roots)
    elif n == 1:
        return external_roots[0]
    else:
        return "."

def _check_if_protos_are_generated(ctx):
    generated_path = ctx.var["GENDIR"]
    print(generated_path)
    print(ctx.attr.src.files.to_list())
    for plfile in ctx.attr.src.files.to_list():
        print(plfile.path)
        print(plfile.is_source)
        if not plfile.path.startswith(generated_path):
            return False
        if not plfile.is_source:
            return False
    return True

def _get_offset_path(root, path):
    """Adjust path relative to offset"""

    if path.startswith("/"):
        fail("path argument must not be absolute: %s" % path)

    if not root:
        return path

    if root == ".":
        return path

        # "external/foobar/file.proto" --> "file.proto"  if path.startswith(root):
        start = len(root)
        if not root.endswith("/"):
            start += 1
            return path[start:]

    depth = root.count("/") + 1
    return "../" * depth + path

def proto_generate_impl(ctx):
    execdir = _get_external_root(ctx)
    if _check_if_protos_are_generated(ctx):
        external = "" if execdir == "." else "/" + execdir
        execdir = ctx.var["GENDIR"] + external
    protoc = _get_offset_path(execdir, ctx.executable.protoc.path)
    plugin = _get_offset_path(execdir, ctx.executable.plugin.path)
    print(ctx.executable.plugin.path)
    print(plugin)
    dir_out = _get_offset_path(execdir, ctx.genfiles_dir.path)
    proto = ctx.attr.src.files.to_list()[0]

    out_files = [ctx.actions.declare_file(out) for out in ctx.attr.outs]
    dir_out = "%s/%s" % (dir_out, ctx.build_file_path[:-5])
    path = _get_offset_path(execdir, proto.path)
    proto_path = "/".join(path.split("/")[:-1])

    all_files = ctx.attr.src.files.to_list()
    protoc_cmd = [protoc]
    protoc_cmd += ["--proto_path=" + proto_path]
    all_inputs = []

    for pfile in ctx.attr.src.files.to_list():
        all_inputs = all_inputs + [pfile]
        ppath = _get_offset_path(execdir, pfile.path)
        rpath = "/".join(ppath.split("/")[4:])
        if rpath == "":
            continue
        protoc_cmd += ["-I" + rpath + "=" + ppath]
    protoc_cmd += ["--plugin=protoc-gen-PLUGIN=" + plugin]
    protoc_cmd += ["--PLUGIN_out=" + dir_out]
    protoc_cmd += [path]

    cmds = []
    if execdir != ".":
        cmds += ["cd %s" % execdir]
    cmds += [" ".join(protoc_cmd)]
    ctx.actions.run_shell(
        inputs = all_inputs + [ctx.executable.plugin] + [ctx.executable.protoc],
        outputs = out_files,
        command = " && ".join(cmds),
    )
    return struct(files = depset(out_files))

_proto_generate = rule(
    attrs = {
        "src": attr.label(
            mandatory = True,
        ),
        "plugin": attr.label(
            mandatory = True,
            executable = True,
            providers = ["files_to_run"],
            cfg = "target",
        ),
        "outs": attr.string_list(),
        "protoc": attr.label(
            default = Label("//external:protobuf_compiler"),
            executable = True,
            cfg = "target",
        ),
    },
    output_to_genfiles = True,
    implementation = proto_generate_impl,
)

def proto_generate(name, src, plugin, outs = [], protoc = None):
    args = {}
    args.update({
        "name": name,
        "src": src,
        "plugin": plugin,
        "outs": outs,
    })
    if protoc:
        args["protoc"] = protoc
    return _proto_generate(**args)

def cc_proto_plugin(
        name,
        src,
        plugin,
        outs = [],
        deps = [],
        protoc = None,
        **kwargs):
    proto_name = name + "_proto"
    proto_generate(proto_name, src, plugin, outs, protoc)
    native.cc_library(
        name = name,
        srcs = [proto_name],
        deps = depset(deps + ["@com_google_protobuf//:protobuf"]).to_list(),
        **kwargs
    )