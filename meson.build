project('madserv', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++11'])

incdir = include_directories('shogo_src/AppHeaders',
                             'shogo_src/Misc')

cpp = meson.get_compiler('cpp')

dl_dep = cpp.find_library('dl', required: true)

executable('madserv',
           'main.cpp',
           'MADServer.cpp',
           'MADServerIO.cpp',
           include_directories : incdir,
           dependencies: dl_dep)
