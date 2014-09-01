import random
from enum import Enum

from monkey import gen_command, gen_number, gen_str, expint, exprange


class Types(Enum):
    ANY = -1
    QEXPR = 0
    SEXPR = 1
    SYMBOL = 2
    FUNC = 3
    NUM = 4
    STR = 5
    ERR = 6

    # Special:
    COMMAND = 7
    BOOL = 8


class Takes(object):
    def __init__(self, *ltypes, **kwargs):
        self.ltypes = ltypes
        self.repeated = False
        self.optional = kwargs.pop('optional', [])

    def repeat(self):
        """
        r
        :rtype: Takes
        """
        self.repeated = True
        return self

    def make(self):
        values = []

        if self.repeated:
            ltypes = self.ltypes * max(2, expint())
        else:
            ltypes = self.ltypes

        for ltype in ltypes:
            values.append(generators[ltype]())

        if random.random() > 0.5:
            for ltype in self.optional:
                values.append(generators[ltype]())

        return ' '.join(values)


class Lambda(object):
    def make(self):
        args = '{' + ' '.join(gen_str(quotes=False) for _ in exprange()) + '}'
        values = '{' + make_any_command() + '}'

        return args + ' ' + values


class Definition(object):
    def make(self):
        n = expint()
        names = '{' + ' '.join(gen_str(quotes=False) for _ in range(n)) + '}'
        values = ' '.join(make_any() for _ in range(n))

        return names + ' ' + values


class Branch(object):
    def make(self):
        value = make(Types.BOOL)
        tbranch = '{' + make_any_command() + '}'
        fbranch = '{' + make_any_command() + '}'

        if int(random.random()):
            return value + ' ' + tbranch
        else:
            return value + ' ' + tbranch + ' ' + fbranch


class Eval(object):
    def make(self):
        return '{' + make_any_command() + '}'


def make(ltype):
    return generators[ltype]()


def make_any(repeated=False, recurse=True):
    gens = generators.copy()
    gens.pop(Types.ANY)
    if not recurse:
        gens.pop(Types.QEXPR)
        gens.pop(Types.COMMAND)

    if repeated:
        ltypes = [random.choice(gens.keys()) for _ in exprange()]
    else:
        ltypes = [random.choice(gens.keys())]

    values = []

    for ltype in ltypes:
        if ltype == Types.QEXPR:
            # Prevent too deep recursion
            values.append('{' + make_any(repeated=True, recurse=False) + '}')
        else:
            values.append(gens[ltype]())

    return ' '.join(values)


def make_any_command():
    return make_command(random.choice(commands.keys()))


def make_command(cmd):
    return cmd + ' ' + commands[cmd].make()


commands = {
    'eval': Eval(),
    #'load': Takes(Types.STR),  # Not implemented
    'print': Takes(Types.ANY),
    'println': Takes(Types.ANY),
    'repr': Takes(Types.ANY),
    'error': Takes(Types.STR),
    'list': Takes(Types.ANY).repeat(),
    'head': Takes(Types.QEXPR),
    'tail': Takes(Types.QEXPR),
    'join': Takes(Types.QEXPR, Types.QEXPR),
    'cons': Takes(Types.QEXPR, Types.QEXPR),
    '+': Takes(Types.NUM).repeat(),
    '-': Takes(Types.NUM).repeat(),
    '*': Takes(Types.NUM).repeat(),
    '/': Takes(Types.NUM).repeat(),
    '%': Takes(Types.NUM).repeat(),
    '>': Takes(Types.NUM).repeat(),
    '>=': Takes(Types.NUM).repeat(),
    '<': Takes(Types.NUM).repeat(),
    '<=': Takes(Types.NUM).repeat(),
    '==': Takes(Types.ANY, Types.ANY),
    '!=': Takes(Types.ANY, Types.ANY),
    'and': Takes(Types.BOOL, Types.BOOL),
    'or': Takes(Types.BOOL, Types.BOOL),
    'not': Takes(Types.BOOL),
    'if': Branch(),
    'lambda': Lambda(),
    'def': Definition(),
    '=': Definition(),
}


def make_number():
    if random.random() < 0.9:
        return gen_number()
    else:
        # Construct something
        ops = ['+', '-', '*', '/']
        return '(' + make_command(random.choice(ops)) + ')'


generators = {
    Types.ANY: make_any,
    Types.QEXPR: lambda: '{' + make_any(repeated=True) + '}',
    Types.FUNC: gen_command,
    Types.NUM: make_number,
    Types.STR: gen_str,
    Types.COMMAND: lambda: '(' + make_any_command() + ')',
    Types.BOOL: lambda: str(random.choice([0, 1]))
}


if __name__ == '__main__':
    for i in range(1000):
        print '(' + make_any_command() + ')'
    # for cmd in commands:
    #     print '(' + make_command(cmd) + ')'
