const std = @import("std");
const builtin = @import("builtin");
const ruby = @cImport(@cInclude("ruby/ruby.h"));
const zhtml2pdf = @import("zhtml2pdf.zig");

fn rb_converter_init(...) callconv(.C) ruby.VALUE {
    var ap: std.builtin.VaList = @cVaStart();
    defer @cVaEnd(&ap);

    const result = zhtml2pdf.init_zhtml2pdf();

    if (result != 0 and result != -1) {
        const exception: ruby.VALUE = ruby.rb_exc_new2(
            ruby.rb_eRuntimeError,
            "Failed to init zhtml2pdf",
        );
        ruby.rb_exc_raise(exception);
    }

    return @cVaArg(&ap, ruby.VALUE);
}

fn rb_converter_finalize(
    _: ruby.VALUE,
    _: ruby.VALUE,
    _: c_int,
    _: [*c]const ruby.VALUE,
    _: ruby.VALUE,
) callconv(.C) ruby.VALUE {
    _ = zhtml2pdf.deinit_zhtml2pdf();
    return ruby.Qnil;
}

fn value2cstring(value: ruby.VALUE) [*c]u8 {
    if (ruby.TYPE(value) == ruby.T_STRING) {
        const v: [*c]volatile ruby.VALUE = @constCast(@ptrCast(&value));
        return ruby.rb_string_value_cstr(v);
    }
    return @constCast(@ptrCast(""));
}

fn rb_converter_convert(...) callconv(.C) ruby.VALUE {
    var ap: std.builtin.VaList = @cVaStart();
    defer @cVaEnd(&ap);

    const self: ruby.VALUE = @cVaArg(&ap, ruby.VALUE);
    _ = self;

    const input: [*c]u8 = value2cstring(@cVaArg(&ap, ruby.VALUE));
    const settings: [*c]u8 = value2cstring(@cVaArg(&ap, ruby.VALUE));
    const css: [*c]u8 = value2cstring(@cVaArg(&ap, ruby.VALUE));

    var content: [*c]u8 = undefined;
    const size: c_int = zhtml2pdf.zhtml2pdf(input, settings, css, &content);
    if (size <= 0) return ruby.INT2NUM(size);

    const rstring: ruby.VALUE = ruby.rb_str_new(content, size);

    zhtml2pdf.zhtml2pdf_free(content);
    content = undefined;

    return rstring;
}

export fn Init_zhtml2pdf() void {
    const zhtml2pdf_mod: ruby.VALUE = ruby.rb_define_module("ZHtml2Pdf");
    const converter_klass: ruby.VALUE = ruby.rb_define_class_under(
        zhtml2pdf_mod,
        "Converter",
        ruby.rb_cObject,
    );

    _ = ruby.rb_define_method(converter_klass, "convert", rb_converter_convert, 3);
    _ = ruby.rb_define_method(converter_klass, "initialize", rb_converter_init, 0);

    const proc: ruby.VALUE = ruby.rb_proc_new(rb_converter_finalize, ruby.Qnil);
    _ = ruby.rb_define_finalizer(converter_klass, proc);
}
