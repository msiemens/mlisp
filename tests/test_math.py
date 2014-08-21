from testhelpers import *
init()

def test_integer():
    with run('+ 5 6') as r:
        assert is_number(r, 11)

    with run('- (* 10 10) (+ 1 1 (/ 2 2))') as r:
        assert is_number(r, 97)

    with run('- 1') as r:
        assert is_number(r, -1)

    with run('% 17 12') as r:
        assert is_number(r, 5)


def test_float():
    with run('/ 1 2') as r:
        assert is_number(r, 0.5)

    with run('+ 1.5 2') as r:
        assert is_number(r, 3.5)

    with run('- 0.2 10') as r:
        assert is_number(r, -9.8)

    with run('* 0.1 10') as r:
        assert is_number(r, 1)

    with run('== 0.1 0.1') as r:
        assert is_number(r, 1)

    with run('== 0.1 0.0999') as r:
        assert is_number(r, 0)


def test_errors():
    with run('/ 10 0') as r:
        assert is_error(r, 'Division by zero')

    with run('% 10 0') as r:
        assert is_error(r, 'Division by zero')

    with run('* % /') as r:
        assert is_error(r, 'Function \'*\' passed incorrect argument types. '
                           'Expected number, got function.')
