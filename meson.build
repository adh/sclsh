project('sclsh', 'c',
        default_options : ['c_std=c11'])

libedit = dependency('libedit', required : true, include_type : 'system')

libsclsh = library('sclsh',
    'src/sclsh.c',
    'src/util.c',
    'src/parse.c',
    'src/ast.c',
    'src/value.c',
    'src/parse.c',
    'src/unwind.c',
    'src/expr.c',
    'src/commands.c',
    include_directories : include_directories('include'),
    install : true,
)

sclsh = executable('sclsh',
    'bin/sclsh.c',
    link_with : libsclsh,
    include_directories : include_directories('include'),
    dependencies : [libedit],
    install : true,
)

install_headers(
    'include/sclsh/sclsh.h',
    'include/sclsh/util.h',
    'include/sclsh/value.h',
    'include/sclsh/ast.h',
    'include/sclsh/unwind.h',
    'include/sclsh/parse.h',
    'include/sclsh/expr.h',
    'include/sclsh/commands.h',
    subdir : 'sclsh'
)