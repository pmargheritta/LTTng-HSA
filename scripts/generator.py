#!/usr/bin/python3

import re

f = open('/home/paul/Source/rocm/ROCR-Runtime/src/core/inc/hsa_internal.h')
header = f.read()

# Select the contents of the namespace
declarations = re.search('namespace\s*HSA\s*\{(.+)\}', header, re.DOTALL).group(1)

###----------------------------
### Generation of new functions
###----------------------------

# Find all the API function declarations
prototypes = re.findall('(\w+)\s+HSA_API\s+(\w+)\((.*?)\);', declarations, re.DOTALL)

definitions = []
for p in prototypes:
    # Generate the new prototype
    definition = '{} {}({})\n'.format(*p)
    definition += '{\n'

    # Generate the entry tracepoint
    definition += '    tracepoint(hsa_runtime, function_entry, "{}");\n'.format(p[1])

    # Replace the callback definitions with the callback names in the arguments list
    args_code = re.sub('\s*\w*\s*(\w+)\s+\(\*(\w+)\)\(.*?\)', '\\1 \\2', p[2], flags=re.DOTALL)

    # Select the types and names of the arguments
    args = re.findall('\s*\w*\s*\w+\**\s+\**(\w+)', args_code)

    # Define the original function
    definition += '    decltype({0}) *orig = (decltype({0})*) dlsym(RTLD_NEXT, "{0}");\n'.format(p[1])

    # Store the result in a variable
    definition += '    '
    if p[0] != 'void':
        definition += '{} retval = '.format(p[0])

    # Call the original function
    definition += 'orig({});\n'.format(', '.join(args));

    # Generate the exit tracepoint
    definition += '    tracepoint(hsa_runtime, function_exit, "{}");\n'.format(p[1])

    # Generate the return statement
    if p[0] != 'void':
        definition += '    return retval;\n'
    definition += '}\n'

    definitions.append(definition)

definitions_code = '\n'.join(definitions)

###------------------------------
### Substitution in the base code
###------------------------------

code = '''#include "util.h"

{}'''.format(definitions_code)

print(code)
