# Main SCons file

DEBUG = 0

env = Environment(CFLAGS="-g -DEBUG", tools=['default',TOOL_HDRTMPL, TOOL_SHADER])


SConscript('src/SConstruct', variant_dir='build/', exports='env' )

Clean('.', 'build')


