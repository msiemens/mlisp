import re
import os
import platform
import subprocess
from contextlib import contextmanager

from cffi import FFI
ffi = FFI()

if platform.system() == 'Windows':
    lib_suffix = '.dll'
elif platform.system() == 'Linux':
    lib_suffix = '.so'
elif platform.system() == 'Darwin':
    lib_suffix = '.dynlib'
else:
    raise Exception('Unknown sytem: %s' % platform.system())

root   = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
header = os.path.join(root, 'include', 'mlisp.h')
src    = os.path.join(root, 'src')
dll_file = os.path.join(root, 'build', 'libmlisp' + lib_suffix)

if not os.path.exists(dll_file):
    raise Exception('Fatal error: %s does not exist!' % dll_file)

# Prepare header file
defs = subprocess.Popen([
    'cc', '-E', '-P', '-DMLISP_NOINCLUDE', '-I' + src, header],
    stdout=subprocess.PIPE).communicate()[0]
defs = defs.replace('\r\n', '\n');
defs = re.sub('#pragma .*', '', defs)

# Load library
ffi.cdef(defs)
lib = ffi.dlopen(dll_file)

# Initialize environment
parser = None
env = None


def _ptr_to_addr(ptr):
    addr = ffi.cast('int', ptr)
    return hex(int(addr))


class ParserError(Exception):
    pass


class lval(object):
    def __init__(self, obj):
        self.obj = lib.lval_copy(obj)
        self.type = obj.type
        self.type_name = ffi.string(ffi.cast("lval_type", self.type))
        self._repr = '<lval %s>' % self.type_name

        if self.type == lib.LVAL_NUM:
            self.num = obj.num
            self._repr = '<lval num: %i>' % self.num

        elif self.type == lib.LVAL_ERR:
            self.err = ffi.string(obj.err)
            self._repr = '<lval error: "%s">' % self.err

        elif self.type == lib.LVAL_STR:
            self.str = ffi.string(obj.str)
            self._repr = '<lval string: "%s">' % self.str

        elif self.type in (lib.LVAL_QEXPR, lib.LVAL_SEXPR):
            rname = 'qexpr' if self.type == lib.LVAL_QEXPR else 'sexpr'
            self._repr = '<lval %s: %s>' % (rname, self)
            self.count = obj.count
            self.values = [lval(obj.values[i]) for i in range(self.count)]

        elif self.type == lib.LVAL_FUNC:
            if obj.builtin:
                self._repr = '<lval builtin: %s>' % _ptr_to_addr(obj.builtin)
                self.builtin = obj.builtin
                self.env = self.formals = self.body = None
            else:
                self._repr = '<lval func: %s>' % self
                self.builtin = None
                self.env = obj.env
                self.formals = obj.formals
                self.body = obj.body

        else:
            raise NotImplementedError('Type %s not yet implemented' % self.type_name)

    def __repr__(self):
        return self._repr

    def __str__(self):
        return ffi.string(lib.lval_to_str(env, self.obj))

#################################################################################
# Initialization

def init():
    global parser
    parser = lib.parser_get()
    init_env()


def init_env():
    global env
    env = lib.lenv_new()
    lib.builtins_init(env)


def reset_env():
    lib.lenv_del(env)
    init_env()

#################################################################################
# Run commands

@contextmanager
def run(line):
    # Cannot use ffi.addressof, therefore create the indirection and
    # dereference later
    result = ffi.new('lval * *')
    parser_error = ffi.new('mpc_err_t * *')

    if lib.parse('<python>', line, env, result, parser_error):
        yield lval(result[0])  # <-- dereference of p_result
    else:
        try:
            raise ParserError(ffi.string(lib.mpc_err_string(parser_error[0])))
        finally:
            lib.mpc_err_delete(parser_error[0])


def run_single(line):
    with run(line) as r:
        return r

#################################################################################
# Test helpers

def is_number(x, val=None):
    if not x.type == lib.LVAL_NUM:
        return False

    if val:
        return round(x.num, 3) == val
    else:
        return True


def is_string(x, val=None):
    if not x.type == lib.LVAL_STR:
        return False

    if val:
        return x.str == val
    else:
        return True


def is_error(x, val=None):
    if not x.type == lib.LVAL_ERR:
        return False

    if val:
        return x.err == val
    else:
        return True


def is_qexpr(x):
    return x.type == lib.LVAL_QEXPR


def is_int_list(x, values):
    if not all(is_number(x.values[i]) for i in range(x.count)):
        return False

    if values:
        if not len(values) == x.count:
            return False

        return all(values[i] == x.values[i].num for i in range(x.count))


def is_sexpr(x):
    return x.type == lib.LVAL_SEXPR


def is_empty(x):
    return x.count == 0


def is_func(x, val=None):
    if not x.type == lib.LVAL_FUNC:
        return False

    if val:
        return x.builtin == val
    else:
        return True

__all__ = ('lib', 'init', 'reset_env', 'run', 'run_single',
           'is_number', 'is_string', 'is_error', 'is_qexpr', 'is_sexpr',
           'is_func', 'is_empty', 'is_int_list')
