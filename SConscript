proj = Environment()
platform = proj['PLATFORM']

proj.Replace(CCFLAGS = '-std=c99 -pedantic')

source_c = ['main.c', 'myjit/jitlib-core.c']

Decider('timestamp-newer')

proj.Program('jitcode', source_c)
