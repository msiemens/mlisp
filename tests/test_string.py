from testhelpers import *
init()

def test_string():
    with run('"hello"') as r:
        assert is_string(r, 'hello')

    with run('"hello\\""') as r:
        assert is_string(r, 'hello"')

    with run('"hello\\n"') as r:
        assert is_string(r, 'hello\n')


def test_comments():
    with run('5 ; this is a comment') as r:
        assert is_number(r, 5)