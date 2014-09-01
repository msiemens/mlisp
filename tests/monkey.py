import random
import string

commands = 'eval load print println repr error list head tail join cons ' \
           '+ - * / % > >= < <= == != and or not if lambda def ='.split()


def exprange(factor=1):
    return range(0, expint(factor))


def expint(factor=1):
    return int(random.expovariate(2 ** factor) * (10 ** factor)) + 1


def gen_qexpr():
    return '{' + ' '.join(gen_anything() for _ in exprange()) + '}'


def gen_sexpr():
    return '(' + ' '.join(gen_anything() for _ in exprange()) + ')'


def gen_str(quotes=True):
    chars = string.ascii_letters + '.<>!'
    if quotes:
        chars += '"\''
    s = ''.join(random.choice(chars) for _ in exprange(2))
    s = s.replace('"', r'\"')
    s = s.replace('\'', r'\'')

    s.strip(string.digits)

    if quotes:
        return '"' + s + '"'
    else:
        return s


def gen_number():
    val = random.gauss(0, 10**min(expint(), 10))
    if val < 0:
        return '(- ' + str(abs(val)) + ')'
    else:
        return str(val)


def gen_command():
    return random.choice(commands)


def gen_anything():
    if random.random() < 0.1:
        return random.choice([gen_qexpr, gen_sexpr])()
    elif random.random() < 0.55:
        return gen_command()
    else:
        return random.choice([gen_str, gen_number])()


def get_command():
    c = '()'
    while c in ['()', '{}']:
        c = gen_sexpr()
    return c


if __name__ == '__main__':
    for i in xrange(1000):
        cmd = get_command()
        print cmd
    print 'exit'
