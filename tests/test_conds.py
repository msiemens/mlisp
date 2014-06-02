from testhelpers import *
init()

def test_ord():
    with run('> () {}') as r:
        assert is_error(r, 'Function \'>\' passed incorrect argument types. '
                           'Expected number, got S-Expression.')

    with run('> 1') as r:
        assert is_error(r, 'Function \'>\' passed too few arguments. '
                           'Expected at least 2, got 1.')

    # x > y
    with run('> 2 1 0 (-5)') as r:
        assert is_number(r, 1)

    with run('> 2 1 0 7') as r:
        assert is_number(r, 0)

    # x >= y
    with run('>= 2 1 1 0') as r:
        assert is_number(r, 1)

    with run('>= 1 2') as r:
        assert is_number(r, 0)

    # x < y
    with run('< 1 2 3') as r:
        assert is_number(r, 1)

    with run('< 1 0') as r:
        assert is_number(r, 0)

    # x <= y
    with run('<= 1 2 2 3') as r:
        assert is_number(r, 1)

    with run('<= 1 0') as r:
        assert is_number(r, 0)


def test_eq():
    for op, (res, n_res) in zip(['==', '!='], [(1, 0), (0, 1)]):

        with run('%s 1 {}' % op) as r:
            assert is_number(r, n_res)

        with run('%s 1 1' % op) as r:
            assert is_number(r, res)

        with run('%s = =' % op) as r:
            assert is_number(r, res)

        with run('%s = *' % op) as r:
            assert is_number(r, n_res)

        with run('%s {1 2} {1 2}' % op) as r:
            assert is_number(r, res)

        with run('%s {1 2} {1 2 3}' % op) as r:
            assert is_number(r, n_res)


def test_if():
    with run('if 1 {5} {9}') as r:
        assert is_number(r, 5)

    with run('if 0 {5} {9}') as r:
        assert is_number(r, 9)

    with run('if 1 {5}') as r:
        assert is_number(r, 5)

    with run('if 0 {5}') as r:
        assert is_sexpr(r) and is_empty(r)

    with run('if (and 1 2) {1} {0}') as r:
        assert is_number(r, 1)

    with run('if (and 1 0) {1} {0}') as r:
        assert is_number(r, 0)

    with run('if (or 0 1) {1} {0}') as r:
        assert is_number(r, 1)

    with run('if (or 0 0) {1} {0}') as r:
        assert is_number(r, 0)