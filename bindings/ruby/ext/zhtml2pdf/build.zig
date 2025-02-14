const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const lib = b.addSharedLibrary(
        .{
            .name = "zhtml2pdf",
            .root_source_file = b.path("src/main.zig"),
            .version = .{ .major = 0, .minor = 0, .patch = 1 },
            .optimize = optimize,
            .target = target,
        },
    );

    const ruby_libdir = std.posix.getenv("RUBY_LIBDIR") orelse "";
    lib.addIncludePath(std.Build.LazyPath{ .cwd_relative = ruby_libdir });
    const ruby_hdrdir = std.posix.getenv("RUBY_HDRDIR") orelse "";
    lib.addIncludePath(std.Build.LazyPath{ .cwd_relative = ruby_hdrdir });
    const ruby_archhdrdir = std.posix.getenv("RUBY_ARCHHDRDIR") orelse "";
    lib.addIncludePath(std.Build.LazyPath{ .cwd_relative = ruby_archhdrdir });

    lib.addCSourceFile(.{
        .file = b.path("../../../../src/zhtml2pdf.c"),
    });

    lib.linkSystemLibrary("webkit2gtk-4.0");
    lib.linkSystemLibrary("glib-2.0");

    lib.linkLibC();

    b.installArtifact(lib);
}
