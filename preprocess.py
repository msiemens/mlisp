from os import path
import os
import re

BASE_PATH = path.abspath(path.dirname(__file__))
SRC_DIR = path.join(BASE_PATH, 'src/')
GEN_DIR = path.join(BASE_PATH, 'gen/')

if not path.exists(GEN_DIR):
    os.mkdir(GEN_DIR)

COMMENT = '"" /*__GRAMMAR__*/'
COMMENT_ESCAPED = re.escape(COMMENT)

GRAMMAR_DEFINITION = path.join(SRC_DIR, 'grammar.ebnf')
PARSER_SOURCE = path.join(SRC_DIR, 'parser.c')
PARSER_DEST = path.join(GEN_DIR, 'parser.c')

with open(PARSER_SOURCE) as f:
    data = f.read()

with open(GRAMMAR_DEFINITION) as f:
    grammar = f.read()
    grammar = re.sub('#.*', '', grammar).strip()
    grammar = re.sub('\\n(\\n)+', '\n', grammar).strip()
    grammar = grammar.replace('\\', '\\\\\\\\')  # Escape backslashes
    grammar = grammar.replace('"', '\\"')  # Escape quotes
    # Add backslash at end of every line except the last
    grammar = '\n'.join('{}\\'.format(l) for l in grammar.splitlines())
    grammar = grammar.strip('\\')
    grammar = '"' + grammar + '"'  # Surround with quotes

pattern = COMMENT_ESCAPED

data = re.sub(pattern, grammar, data)

with open(PARSER_DEST, 'w') as f:
    f.write(data)
