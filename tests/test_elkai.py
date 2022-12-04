from pathlib import Path
import pytest
import numpy as np
import elkai

def read_symmetric_instance():
    path = Path(__file__).parent.absolute() / "kroA100.tsp"
    node_coords_section_begin = False
    nodes = []
    with open(path.absolute(), "r") as f:
        for line in f:
            if line.startswith("NODE_COORD_SECTION"):
                node_coords_section_begin = True
                continue
            if line.startswith("EOF"):
                break
            if node_coords_section_begin:
                idx, x, y = line.split()
                idx = int(idx)
                x = float(x)
                y = float(y)
                nodes.append((idx, x, y))
    nodes.sort()
    assert nodes[-1][0] == len(nodes), "Node indices must be consecutive"
    # calculate distance matrix
    def _euc_2d(a, b):
        return int(np.sqrt((a[1] - b[1]) ** 2 + (a[2] - b[2]) ** 2) + 0.5)
    M = np.zeros((len(nodes), len(nodes)), dtype=int)
    for i in range(len(nodes)):
        M[i][i] = 0
        for j in range(i + 1, len(nodes)):
            M[i, j] = _euc_2d(nodes[i], nodes[j])
            M[j, i] = M[i, j]
    return M

def read_asymmetric_instance():
    path = Path(__file__).parent.absolute() / "ftv100.atsp"
    edge_weight_section_begin = False
    weight_matrix = []
    current_node_weights = []
    with open(path.absolute(), "r") as f:
        for line in f:
            if line.startswith("DIMENSION"):
                dimension = int(line.split()[1])
            if line.startswith("EDGE_WEIGHT_SECTION"):
                edge_weight_section_begin = True
                continue
            if line.startswith("EOF"):
                break
            if edge_weight_section_begin:
                weight = int(line)
                current_node_weights.append(weight)
                if len(current_node_weights) == dimension:
                    weight_matrix.append(current_node_weights.copy())
                    current_node_weights.clear()
    assert current_node_weights == [], "Incomplete weight matrix"
    M = np.array(weight_matrix)
    return M

def get_tour_length(tour, matrix, bottleneck=False):
    tour_length = 0
    for i in range(len(tour)):
        dist = matrix[tour[i], tour[(i + 1) % len(tour)]]
        if bottleneck:
            tour_length = max(tour_length, dist)
        else:
            tour_length += dist
    return tour_length

@pytest.mark.parametrize("bottleneck", [False, True])
def test_symmetric(bottleneck):
    M = read_symmetric_instance()
    tour = elkai.solve_int_matrix(M, bottleneck=bottleneck)
    tour_length = get_tour_length(tour, M, bottleneck=bottleneck)
    if not bottleneck:
        # compare with standalone LKH solution
        assert tour_length == 21282
    else:
        # compare with standalone BLKH solution
        assert tour_length == 475

@pytest.mark.parametrize("bottleneck", [False, True])
def test_asymmetric(bottleneck):
    M = read_asymmetric_instance()
    tour = elkai.solve_int_matrix(M, bottleneck=bottleneck, tracelevel=0)
    tour_length = get_tour_length(tour, M, bottleneck=bottleneck)
    if not bottleneck:
        # compare with standalone LKH solution
        assert tour_length == 1788
    else:
        # compare with standalone BLKH solution
        assert tour_length == 53

if __name__ == "__main__":
    # pytest.main([__file__])
    test_symmetric(bottleneck=False)
    test_symmetric(bottleneck=True)
    test_asymmetric(bottleneck=False)
    test_asymmetric(bottleneck=True)