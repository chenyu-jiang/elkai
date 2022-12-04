from ._elkai import solve
import struct
import os

LKH_PATH = os.path.join(os.path.split(__file__)[0], "LKH_EXEC")
assert os.path.isfile(LKH_PATH), "LKH executable not found."

def float_to_int(x):
    """Converts a float into some unknown integer such that
    the order is preserved: x <= y <=> f(x) <= f(y)"""
    x_sign = int(x < 0)

    int_repr = struct.unpack("<i", struct.pack("<f", abs(x / 100)))[0]
    SQUEEZE_FACTOR = 0.7
    int_sq = int(abs(int_repr) ** SQUEEZE_FACTOR) * ((-1) ** x_sign)
    return int_sq


def flatten_matrix(mat, mat_type=int):
    MAXINT = 2147483647
    N = len(mat)
    if N < 2:
        raise TypeError("Argument must be a N*N matrix with N > 1")
    flattened = []
    for row in mat:
        if len(row) != N:
            raise TypeError("Argument must be a N*N matrix")
        for column in row:
            int_col = int(column)
            if int_col > MAXINT:
                raise TypeError("Distance must be < 2^31. Try the float version")
            flattened.append(mat_type(column))
    return flattened

def solve_int_matrix(matrix, runs=10, bottleneck=False, tracelevel=0):
    flat_mat = flatten_matrix(matrix)
    return solve(flat_mat, runs, 1 if bottleneck else 0, tracelevel, LKH_PATH)


def solve_float_matrix(matrix, runs=10, bottleneck=False, tracelevel=0):
    flat_mat = [float_to_int(float(x)) for x in flatten_matrix(matrix, mat_type=float)]
    return solve(flat_mat, runs, 1 if bottleneck else 0, tracelevel, LKH_PATH)
