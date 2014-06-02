from testhelpers import *
init()


def test_list():
    with run('list 1 2 3') as r:
        assert is_qexpr(r)

        for i, v in enumerate(r.values):
            assert is_number(v, i + 1)


def test_head():
    with run('head {1 2 3}') as r:
        assert is_qexpr(r) and is_int_list(r, [1])

    with run('head {1} {1}') as r:
        assert is_error(r, 'Function \'head\' passed too many arguments. '
                           'Expected 1, got 2.')

    with run('head 1') as r:
        assert is_error(r, 'Function \'head\' passed incorrect argument types. '
                           'Expected Q-Expression, got number.')

    with run('head {}') as r:
        assert is_error(r, 'Function \'head\' passed empty list.')


def test_tail():
    with run('tail {1 2 3}') as r:
        assert is_qexpr(r) and is_int_list(r, [2, 3])

    with run('tail {1} {1}') as r:
        assert is_error(r, 'Function \'tail\' passed too many arguments. '
                           'Expected 1, got 2.')

    with run('tail 1') as r:
        assert is_error(r, 'Function \'tail\' passed incorrect argument types. '
                           'Expected Q-Expression, got number.')

    with run('tail {}') as r:
        assert is_error(r, 'Function \'tail\' passed empty list.')


def test_eval():
    with run('eval {+ 1 2}') as r:
        assert is_number(r, 3)

    with run('eval {1} {1}') as r:
        assert is_error(r, 'Function \'eval\' passed too many arguments. '
                           'Expected 1, got 2.')

    with run('eval 1') as r:
        assert is_error(r, 'Function \'eval\' passed incorrect argument types. '
                           'Expected Q-Expression, got number.')


def test_join():
    with run('join {1} {2}') as r:
        assert is_qexpr(r) and is_int_list(r, [1, 2])

    with run('join 1') as r:
        assert is_error(r, 'Function \'join\' passed incorrect argument types. '
                           'Expected Q-Expression, got number.')


def test_cons():
    with run('cons {1} {2}') as r:
        assert is_qexpr(r)
        assert is_qexpr(r.values[0])
        assert r.values[0].values[0].num == 1
        assert r.values[1].num == 2

    with run('cons {1}') as r:
        assert is_error(r, 'Function \'cons\' passed too few arguments. '
                           'Expected 2, got 1.')

    with run('cons {1} {1} {1}') as r:
        assert is_error(r, 'Function \'cons\' passed too many arguments. '
                           'Expected 2, got 3.')

    with run('cons 1 1') as r:
        assert is_error(r, 'Function \'cons\' passed incorrect argument types. '
                           'Expected Q-Expression, got number.')


def test_lambda():
    with run('lambda {x y} {+ x y}') as r:
        assert is_func(r)

    with run('(lambda {x y} {+ x y}) 10 20') as r:
        assert is_number(r, 30)

    #

    with run('def {add-mul} (lambda {x y} {+ x (* x y)})') as r:
        assert is_sexpr(r)

    with run('add-mul 10 20') as r:
        assert is_number(r, 210)

    with run('add-mul 10') as r:
        assert is_func(r)

    with run('(add-mul 10) 50') as r:
        assert is_number(r, 510)

    #

    with run('(lambda {x ... a} {a})') as r:
        assert is_func(r)

    with run('(lambda {x ... a} {a}) 1') as r:
        assert is_qexpr(r) and is_empty(r)

    with run('(lambda {x ... a} {a}) 1 2') as r:
        assert is_qexpr(r) and is_int_list(r, [2])

    with run('(lambda {x eval} {}) 1 2') as r:
        assert is_error(r, 'Cannot redefine builtin \'eval\'.')

    with run('(lambda {x ... eval} {}) 1 2') as r:
        assert is_error(r, 'Cannot redefine builtin \'eval\'.')

    reset_env()


if __name__ == '__main__':
    test_list()
    test_head()
    test_tail()
    test_eval()
    test_join()
    test_cons()
    test_lambda()