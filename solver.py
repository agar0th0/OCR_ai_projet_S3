def check(grid, trie, i, j, i_diff, j_diff, moves):
    n, m = len(grid), len(grid[0])
    start_i, start_j = i, j
    substring = ''
    node = trie
    while 0 <= i and 0 <= j < m and grid[i][j] in node.children:
        substring += grid[i][j]
        node = node.children[grid[i][j]]
        if node.is_end:
            trie.delete(substring)
            moves.append(((start_i, start_j), (i, j)))

def solve():
    n, m = len(grid), len(grid[0])
    moves = []
    trie = Trie().build(words)
    directions = [(0,1),(1,1),(1,0),(1,-1),(0,-1),(-1,-1),(-1,0),(-1,1)]
    for i in range(n):
        for j in range(m):
            if grid[i][j] in trie.children:
                for i_diff, j_diff in directions:
                    check(grid, trie, i, j, i_diff, j_diff, moves)

    return moves
