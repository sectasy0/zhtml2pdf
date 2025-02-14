const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const lib = b.addStaticLibrary(.{
        .name = "zhtml2pdf",
        .optimize = optimize,
        .target = target,
        .version = .{ .major = 0, .minor = 0, .patch = 1 },
    });

    lib.addCSourceFile(.{
        .file = b.path("src/zhtml2pdf.c"),
    });

    lib.linkSystemLibrary("webkit2gtk-4.0");
    lib.linkSystemLibrary("glib-2.0");

    lib.linkLibC();

    b.installArtifact(lib);
}
