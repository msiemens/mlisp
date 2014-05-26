from os import path
import re

BASE_PATH = path.abspath(path.dirname(__file__))
SRC_DIR = path.join(BASE_PATH, 'src/')
GEN_DIR = path.join(BASE_PATH, 'gen/')

COMMENT = '/*__GRAMMAR__*/'
COMMENT_ESCAPED = re.escape(COMMENT)

GRAMMAR_DEFINITION = path.join(SRC_DIR, 'grammar.ebnf')
GRAMMAR_SOURCE = path.join(SRC_DIR, 'grammar.c')
GRAMMAR_DEST = path.join(GEN_DIR, 'grammar.c')

with open(GRAMMAR_SOURCE) as f:
    data = f.read()

with open(GRAMMAR_DEFINITION) as f:
    grammar = f.read()
    grammar = re.sub('#.*', '', grammar).strip()
    grammar = grammar.replace('\\', '\\\\\\\\')  # Escape backslashes
    grammar = grammar.replace('"', '\\"')  # Escape quotes
    # Add backslash at end of every line except the last
    grammar = '\n'.join('{}\\'.format(l) for l in grammar.splitlines())
    grammar = grammar.strip('\\')
    grammar = '"' + grammar + '"'  # Surround with quotes

pattern = COMMENT_ESCAPED

data = re.sub(pattern, grammar, data)

with open(GRAMMAR_DEST, 'w') as f:
    f.write(data)
