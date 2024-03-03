project(
    'memblk',
    'c',
    version: '0.1',
    default_options: ['warning_level=3'],
)

subdir('src')
subdir('tests')

memblk = library('memblk', 'src/memblk.c')

unity_dep = dependency('unity')
testex = executable(
    'unit_test',
    'tests/tests.c',
    dependencies: [unity_dep],
    link_with: [memblk],
)

