from testhelpers import *
init()

mlisp_builtins = ["+", "-", "*", "/", "%", "head", "tail", "list",
                  "eval", "join", "cons", "def", "=", "lambda"]


def test_qexpr():
    with run('{}') as r:
        assert is_qexpr(r) and is_empty(r)

    with run('{1 2 3}') as r:
        assert is_qexpr(r)
        assert str(r) == '{1 2 3}'

        for i, v in enumerate(r.values):
            assert is_number(v, i + 1)


def test_define():
    with run('def {x} 100') as r:
        assert is_sexpr(r)

    with run('x') as r:
        assert is_number(r, 100)


def test_unbound():
    with run('y') as r:
        assert is_error(r, 'Unbound symbol: \'y\'')


def test_misc():
    with run('eval (head {+ - + - * /})') as r:
        assert is_func(r, lib.builtin_add)

    #

    with run('def {a b} 5 6') as r:
        assert is_sexpr(r)

    with run('+ a b') as r:
        assert is_number(r, 11)

    #

    with run('def {arglist} {a b}') as r:
        assert is_sexpr(r)

    #

    with run('def arglist 1 2') as r:
        assert is_sexpr(r)

    with run('list a b') as r:
        assert is_qexpr(r)
        assert is_int_list(r, [1, 2])

    reset_env()