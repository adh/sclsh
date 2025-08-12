project('sclsh', 'c',
        default_options : ['c_std=c11'])

libsclsh = library('sclsh',
    'src/sclsh.c',
    'src/util.c',
    'src/parse.c',
    'src/ast.c',
    'src/value.c',
    'src/io.c',
    include_directories : include_directories('include'),
    install : true,
)

sclsh = executable('sclsh',
    'bin/sclsh.c',
    link_with : libsclsh,
    include_directories : include_directories('include'),
    install : true,
)

install_headers(
    'include/sclsh/sclsh.h',
    'include/sclsh/util.h',
    'include/sclsh/value.h',
    'include/sclsh/ast.h',
    'include/sclsh/io.h',
    subdir : 'sclsh'
)